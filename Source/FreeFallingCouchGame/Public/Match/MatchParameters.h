// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Containers/List.h"
#include "MatchParameters.generated.h"
/**
 * 
 */
UENUM()
enum EMatchTypes
{
	Classic,
	TwoVsTwo,
	Special,
};

UENUM()
enum EWinConditions
{
	OneRemaining,
	Timed,
};

UCLASS(Blueprintable)
class FREEFALLINGCOUCHGAME_API UMatchParameters : public UObject
{
	GENERATED_BODY()
	public:
	const int* getScoreValues() const;
	int getMaxRounds() const;
	float getTimerDelay() const {return EventDelay;}
	FString getEraChosen() const {return EraName;}
	void setValues(TObjectPtr<UMatchParameters> UserParameters);
protected:
	// Scores based on your ranking (IN ORDER)
	UPROPERTY(EditAnywhere)
	int ScoreValue[4] = {1000, 500, 300, 100};
	// Number of rounds per match
	UPROPERTY(EditAnywhere)
	int MaxRounds = 5;
	// Delay between which events are summoned, if set to 0, events are disabled
	UPROPERTY(EditAnywhere)
	float EventDelay = 55.0f;
	// Selected Era name
	UPROPERTY(EditAnywhere)
	FString EraName = "Default";
	// Current Match Type (Classic, Teams, etc...)
	//EMatchTypes MatchType;
};