// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlayerMatchData.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UPlayerMatchData : public UObject
{
	GENERATED_BODY()
public:
	const TArray<int> getScoreValues() const {return PlayerScores;};
	void setScoreValue(int InID, int InScore)
	{
		PlayerScores[InID - 1] += InScore;
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Yellow,
FString::Printf(TEXT("Added %i points to player id %i !"), InScore, InID));
	};
	void resetScoreValue()
	{
		PlayerScores.Empty();
		for (int x = 0; x < 4; x++)
		{
			PlayerScores.Add(0);
		}
	};

	void DebugPrintScore()
	{
		int i = 1;
		for(int PlayerScore : PlayerScores)
		{
			if(PlayerScore > 0)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan,
		FString::Printf(TEXT("Player: %i  has finished the match with %i points!"), i, PlayerScore));
				i++;
			}
		}
	}
protected:
	UPROPERTY(VisibleAnywhere)
	TArray<int> PlayerScores;
	// And here could be stored other data such as who held an umbrella the longest etc etc
};
