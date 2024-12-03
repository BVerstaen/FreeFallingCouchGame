// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "VictorySceneGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API AVictorySceneGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	TArray<int> GetWinners();

	UFUNCTION(BlueprintCallable)
	const int GetPlayerIDFromScorePosition(int ScorePosition);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGotWinner, const TArray<int>&, WinnerList);
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnGotWinner OnGotWinner;

	UPROPERTY(BlueprintReadWrite)
	TArray<int> ScoreList;

	UFUNCTION(BlueprintCallable)
	FString FormatWinningPlayers(const TArray<int>& WinningPlayers);

	inline static bool SortPredicate(int itemA, int itemB);
};
