// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/TrackerActor.h"

#include "Characters/FreeFallCharacter.h"

FPlayerTrackedData::FPlayerTrackedData() {}
FPlayerTrackedData::FPlayerTrackedData(const int ID) : PlayerId(ID){}

ATrackerActor::ATrackerActor(): CurrentParachuteHolderIndex(-1)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATrackerActor::Init(AParachute* ParachuteInstance, TArray<AFreeFallCharacter*>& CharactersInsideArena)
{
	//Guard conditions
	if(!ParachuteInstance)
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

		Character->OnWasEliminated.AddDynamic(this, &ATrackerActor::AddNbOfElimination);
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
		PlayerTrackedDataList[CurrentParachuteHolderIndex - 1].TimeWithParachute += DeltaTime;
	}
}

TArray<int> ATrackerActor::GetTrackingWinners(ETrackingRewardCategory Category)
{
	TArray<int> WinningPlayer;
	int HighestNumberInt = 0;
	float HighestNumberFloat = 0.f;
	
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
			//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::FromInt(TrackedData.PlayerId) + TEXT(" - ") + FString::FromInt(TrackedData.NbOfElimination));
			
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

void ATrackerActor::AddNbOfElimination(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NextOwner)
{
	for(FPlayerTrackedData& TrackedData : PlayerTrackedDataList)
	{
		if(TrackedData.PlayerId == NextOwner->getIDPlayerLinked())
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
		//Check if category contains name
		if (!CategoryNames.Contains(Category)) continue;
    
		//Display reward name
		FString RewardMessage = "Reward:" + CategoryNames[Category];

		
		TArray<int> WinningPlayers = GetTrackingWinners(Category);
		if (WinningPlayers.Num() > 0)
		{
			RewardMessage += " - Winner(s): ";
			for (int PlayerId : WinningPlayers)
			{
				RewardMessage += FString::FromInt(PlayerId) + " ";
			}
		}
		else
		{
			RewardMessage += " - No winner";
		}
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, RewardMessage);
	}
}

void ATrackerActor::RemoveDelegates()
{
	if(CurrentParachute)
	{
		CurrentParachute->OnParachuteStolen.RemoveDynamic(this, &ATrackerActor::AddNbOfStealParachute);
		CurrentParachute->OnParachuteGrabbed.RemoveDynamic(this, &ATrackerActor::StartParachuteTracking);
		CurrentParachute->OnParachuteStolen.RemoveDynamic(this, &ATrackerActor::ChangeParachuteTracking);
		CurrentParachute->OnParachuteDropped.RemoveDynamic(this, &ATrackerActor::ATrackerActor::StopParachuteTracking);
	}
}

TArray<int> ATrackerActor::GiveWinners()
{
	TArray<int> WinnersList;
	
	//Categories of awards
	TArray<ETrackingRewardCategory> Categories = {
		MostStealParachute,
		LongestTimeWithParachute,
		MostElimination,
		MostBonusUsed
	};

	for(ETrackingRewardCategory Category : Categories)
	{
		TArray<int> CategoryWinners = GetTrackingWinners(Category);
		WinnersList.Append(CategoryWinners);
	}

	return WinnersList;
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

