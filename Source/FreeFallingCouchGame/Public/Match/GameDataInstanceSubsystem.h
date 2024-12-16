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
	UPROPERTY(BlueprintReadOnly)
	int CurrentRound = 0;

	UPROPERTY(BlueprintReadWrite)
	bool IsFinalRound = false;
	
	int NextParachuteHolderID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentColorVisionDeficiency;

#pragma region PlayerCostume
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDCostumeSwitch, int, IDCostume, int, IDplayer);
	UPROPERTY(BlueprintAssignable,BlueprintCallable, Category = "EventsCostume")
	FDCostumeSwitch OnCostumeSwitch;
    
   	//Get Costume ID from PlayerID, returns -1 if not found
   	UFUNCTION(BlueprintCallable)
   	int GetPlayerLookFromID(int PlayerID);
   
   	//Set Player Costume ID from PlayerID, creates one if necessary
   	UFUNCTION(BlueprintCallable)
   	void SetPlayerLookFromID(int PlayerID, int LookID);

	//Reset Player costumes link
	UFUNCTION(BlueprintCallable)
	void ResetPlayerCostumes();
	
protected:
	// <Player ID, Costume ID>
	UPROPERTY(BlueprintReadOnly)
	TMap<int, int> PlayerCostumeMap;
#pragma endregion

#pragma region PlayerScore
protected:
	UPROPERTY()
	TMap<int, int> PlayerScoreFromID;
	
public:
	//Get PlayerID from Player score, returns -1 if not found
	const int* GetPlayerIDFromScore(int Score);
	
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
