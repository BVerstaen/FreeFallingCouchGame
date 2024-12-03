// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/VictorySceneGameMode.h"

#include "Audio/SoundSubsystem.h"
#include "Match/GameDataInstanceSubsystem.h"

void AVictorySceneGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameDataInstanceSubsystem* GameDataInstanceSubsystem = GetGameInstance()->GetSubsystem<UGameDataInstanceSubsystem>();

	ScoreList =
	{
		GameDataInstanceSubsystem->GetPlayerScoreFromID(0),
		GameDataInstanceSubsystem->GetPlayerScoreFromID(1),
		GameDataInstanceSubsystem->GetPlayerScoreFromID(2),
		GameDataInstanceSubsystem->GetPlayerScoreFromID(3)
	};
	
	TArray<int> WinnersList = GetWinners();
	OnGotWinner.Broadcast(WinnersList);

	//Play win music
	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if(WinnersList.Num() > 0)
	{
		SoundSubsystem->PlayMusic("MUS_Victory_ST_Loop");
	}
	else
	{
		SoundSubsystem->PlayMusic("MUS_Lose_ST_Loop");
	}
}

TArray<int> AVictorySceneGameMode::GetWinners()
{
	TArray<int> WinnersList;

	int HighestScore = 0;
	int CurrentPlayerID = 0;
	for(int Score : ScoreList)
	{
		//In case of equality
		if(Score == HighestScore && HighestScore > 0)
		{
			WinnersList.Add(CurrentPlayerID);
		}
		//In case of highest number
		else if(Score > HighestScore)
		{
			WinnersList.Empty();
			WinnersList.Add(CurrentPlayerID);
			HighestScore = Score;
		}

		CurrentPlayerID++;
	}

	return WinnersList;
}

const int AVictorySceneGameMode::GetPlayerIDFromScorePosition(int ScorePosition)
{
	UGameDataInstanceSubsystem* GameDataInstanceSubsystem = GetGameInstance()->GetSubsystem<UGameDataInstanceSubsystem>();
	ScoreList =
	{
		GameDataInstanceSubsystem->GetPlayerScoreFromID(0),
		GameDataInstanceSubsystem->GetPlayerScoreFromID(1),
		GameDataInstanceSubsystem->GetPlayerScoreFromID(2),
		GameDataInstanceSubsystem->GetPlayerScoreFromID(3)
	};
	ScoreList.Sort(SortPredicate);
	
	return *GameDataInstanceSubsystem->GetPlayerIDFromScore(ScoreList[ScorePosition]);
}

FString AVictorySceneGameMode::FormatWinningPlayers(const TArray<int>& WinningPlayers)
{
	if (WinningPlayers.Num() == 0)
	{
		return TEXT("No players win!"); //If there's no winner
	}

	FString Result;

	//if only one winner
	if (WinningPlayers.Num() == 1)
	{
		Result = FString::Printf(TEXT("Player %d wins!"), WinningPlayers[0] + 1);
	}
	else
	{
		//Sevral winners
		for (int32 i = 0; i < WinningPlayers.Num(); ++i)
		{
			if (i == WinningPlayers.Num() - 1)
			{
				//If last player -> add "and"
				Result += FString::Printf(TEXT("and %d"), WinningPlayers[i] + 1);
			}
			else if (i == WinningPlayers.Num() - 2)
			{
				//If before last player -> don't add a comma
				Result += FString::Printf(TEXT("%d "), WinningPlayers[i] + 1);
			}
			else
			{
				//else add comma
				Result += FString::Printf(TEXT("%d, "), WinningPlayers[i] + 1);
			}
		}
		
		//add prefix & suffix
		Result = FString::Printf(TEXT("Players %s win!"), *Result);
	}

	return Result;
}

bool AVictorySceneGameMode::SortPredicate(int itemA, int itemB)
{
	return (itemA > itemB);
}
