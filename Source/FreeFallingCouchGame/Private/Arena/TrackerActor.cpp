﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/TrackerActor.h"

#include "Characters/FreeFallCharacter.h"

FPlayerTrackedData::FPlayerTrackedData() {}
FPlayerTrackedData::FPlayerTrackedData(const int ID) : PlayerId(ID){}

void ATrackerActor::Init(AParachute* ParachuteInstance, TArray<AFreeFallCharacter*>& CharactersInsideArena)
{
	//Guard conditions
	if(ParachuteInstance)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "No parachute in FreeFallGameMode");
		return;
	}
	if(CharactersInsideArena.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "No Characters Spawned in FreeFallGameMode");
		return;
	}

	CurrentParachute = ParachuteInstance;
	
	//Init players tracked datas
	PlayerTrackedDataList.Empty();
	for(AFreeFallCharacter* Character : CharactersInsideArena)
	{
		//Create player tracked data list
		FPlayerTrackedData TrackedData(Character->getIDPlayerLinked());
		PlayerTrackedDataList.Add(TrackedData);
	}

	//Bind steal parachute
	CurrentParachute->OnParachuteStolen.AddDynamic(this, &ATrackerActor::AddNbOfStealParachute);
	CurrentParachute->OnParachuteGrabbed.AddDynamic(this, &ATrackerActor::StartParachuteTracking);
	CurrentParachute->OnParachuteStolen.AddDynamic(this, &ATrackerActor::ChangeParachuteTracking);
	CurrentParachute->OnParachuteDropped.AddDynamic(this, &ATrackerActor::ATrackerActor::StopParachuteTracking);
}

void ATrackerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TrackHoldParachute(DeltaTime);
}

void ATrackerActor::TrackHoldParachute(float DeltaTime)
{
	if(CurrentParachuteHolderIndex > 0)
	{
		PlayerTrackedDataList[CurrentParachuteHolderIndex].TimeWithParachute += DeltaTime;
	}
}

TArray<int> ATrackerActor::GetTrackingWinners(ETrackingRewardCategory Category)
{
	TArray<int> WinningPlayer;
	int HighestNumberInt = -1;
	float HighestNumberFloat = -0.1f;
	
	switch (Category)
	{
	case MostStealParachute:
		for(FPlayerTrackedData TrackedData : PlayerTrackedDataList)
		{
			//In case of equality
			if(TrackedData.NbOfStealParachute == HighestNumberInt && HighestNumberInt > 0)
			{
				WinningPlayer.Add(TrackedData.PlayerId);
			}
			//In case of highest number
			else if(TrackedData.NbOfStealParachute > HighestNumberInt)
			{
				WinningPlayer.Empty();
				WinningPlayer.Add(TrackedData.PlayerId);
				HighestNumberInt = TrackedData.NbOfStealParachute;
			}
		}
		return WinningPlayer;
		
	case LongestTimeWithParachute:
		for(FPlayerTrackedData TrackedData : PlayerTrackedDataList)
		{
			//In case of equality
			if(FMath::IsNearlyEqual(TrackedData.TimeWithParachute,HighestNumberFloat) && HighestNumberFloat > 0.0f)
			{
				WinningPlayer.Add(TrackedData.PlayerId);
			}
			//In case of highest number
			else if(TrackedData.TimeWithParachute > HighestNumberFloat)
			{
				WinningPlayer.Empty();
				WinningPlayer.Add(TrackedData.PlayerId);
				HighestNumberFloat = TrackedData.TimeWithParachute;
			}
		}
		return WinningPlayer;
		
	case MostElimination:
		for(FPlayerTrackedData TrackedData : PlayerTrackedDataList)
		{
			//In case of equality
			if(TrackedData.NbOfElimination == HighestNumberInt && HighestNumberInt > 0)
			{
				WinningPlayer.Add(TrackedData.PlayerId);
			}
			//In case of highest number
			else if(TrackedData.NbOfElimination > HighestNumberInt)
			{
				WinningPlayer.Empty();
				WinningPlayer.Add(TrackedData.PlayerId);
				HighestNumberInt = TrackedData.NbOfElimination;
			}
		}
		return WinningPlayer;
		
	case MostBonusUsed:
		for(FPlayerTrackedData& TrackedData : PlayerTrackedDataList)
		{
			//In case of equality
			if(TrackedData.NbBonusUsed == HighestNumberInt && HighestNumberInt > 0)
			{
				WinningPlayer.Add(TrackedData.PlayerId);
			}
			//In case of highest number
			else if(TrackedData.NbBonusUsed > HighestNumberInt)
			{
				WinningPlayer.Empty();
				WinningPlayer.Add(TrackedData.PlayerId);
				HighestNumberInt = TrackedData.NbBonusUsed;
			}
		}
		return WinningPlayer;
		
	default:
		return WinningPlayer;
	}
}

void ATrackerActor::AddNbOfStealParachute(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NewOwner)
{
	for(FPlayerTrackedData& TrackedData : PlayerTrackedDataList)
	{
		if(TrackedData.PlayerId == NewOwner->getIDPlayerLinked())
		{
			TrackedData.NbOfStealParachute++;
		}
	}
}

void ATrackerActor::AddNbOfElimination(const AFreeFallCharacter* Character)
{
	for(FPlayerTrackedData& TrackedData : PlayerTrackedDataList)
	{
		if(TrackedData.PlayerId == Character->getIDPlayerLinked())
		{
			TrackedData.NbOfElimination++;
		}
	}
}

void ATrackerActor::AddNbBonusUsed(const AFreeFallCharacter* Character)
{
	for(FPlayerTrackedData& TrackedData : PlayerTrackedDataList)
	{
		if(TrackedData.PlayerId == Character->getIDPlayerLinked())
		{
			TrackedData.NbBonusUsed++;
		}
	}
}

void ATrackerActor::DebugPrintResultReward()
{
	//Categories of awards
	TArray<ETrackingRewardCategory> Categories = {
		MostStealParachute,
		LongestTimeWithParachute,
		MostElimination,
		MostBonusUsed
	};

	//Names of Categories
	TMap<ETrackingRewardCategory, FString> CategoryNames = {
		{MostStealParachute, TEXT("Most Steal Parachute")},
		{LongestTimeWithParachute, TEXT("Longest Time With Parachute")},
		{MostElimination, TEXT("Most Eliminations")},
		{MostBonusUsed, TEXT("Most Bonuses Used")}
	};

	//Get winners and display them in viewport
	for (ETrackingRewardCategory Category : Categories)
	{
		TArray<int> WinningPlayers = GetTrackingWinners(Category);
        
		//Display reward name
		FString RewardMessage = FString::Printf(TEXT("Reward: %s"), *CategoryNames[Category]);

		if (WinningPlayers.Num() > 0)
		{
			RewardMessage += TEXT(" - Winner(s): ");
			for (int PlayerId : WinningPlayers)
			{
				RewardMessage += FString::FromInt(PlayerId) + TEXT(" ");
			}
		}
		else
		{
			RewardMessage += TEXT(" - No winner");
		}
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, RewardMessage);
	}
}

void ATrackerActor::StartParachuteTracking(AFreeFallCharacter* NewOwner)
{
	CurrentParachuteHolderIndex = NewOwner->getIDPlayerLinked();
}

void ATrackerActor::ChangeParachuteTracking(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NewOwner)
{
	CurrentParachuteHolderIndex = NewOwner->getIDPlayerLinked();
}

void ATrackerActor::StopParachuteTracking(AFreeFallCharacter* PreviousOwner)
{
	CurrentParachuteHolderIndex = -1;
}

