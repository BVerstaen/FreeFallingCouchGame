// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MatchParameters.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameDataInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UGameDataInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int CurrentRound = 0;

#pragma region PlayerScore
	
protected:
	UPROPERTY()
	TMap<int, int> PlayerScoreFromID;
	
public:
	//Get Player score from PlayerID, returns -1 if not found
	int GetPlayerScoreFromID(int PlayerID);

	//Set Player score from PlayerID, creates one if necessary
	void SetPlayerScoreFromID(int PlayerID, int PlayerScore);

	//Add to Current Player score from PlayerID, ignore if Current Player Score doesn't exist
	void AddPlayerScoreFromID(int PlayerID, int PointsToAdd);

	void ResetPlayerScore();

#pragma endregion
	
	UFUNCTION()
	UMatchParameters *GetMatchParameters() {return CurrentParameters;}
	UFUNCTION()
	void SetMatchParameters(UMatchParameters *NewMatchParameters)
	{
		if(!NewMatchParameters->IsValidLowLevel())
		{
			NewMatchParameters =  NewObject<UMatchParameters>();
		}
		CurrentParameters = NewMatchParameters;
	}
	UFUNCTION()
	void CreateMatchParameters()
	{
		CurrentParameters =  NewObject<UMatchParameters>();
	}
protected:
	UPROPERTY()
	UMatchParameters *CurrentParameters;
};
