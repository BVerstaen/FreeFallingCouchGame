// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Containers/List.h"
#include "MatchParameters.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum EMatchTypes
{
	Classic,
	Duos,
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
	void Init(TSubclassOf<UMatchParameters> UserParameters);
	const int* getScoreValues() const;
	UFUNCTION(BlueprintCallable)
	int getMaxRounds() const;
	UFUNCTION(BlueprintCallable)
	float getTimerDelay() const {return EventDelay;}
	UFUNCTION(BlueprintCallable)
	FString getEraChosen() const {return EraName;}
	UFUNCTION(BlueprintCallable)
	EMatchTypes getMatchType() const {return MatchType;}
	void setValues(TObjectPtr<UMatchParameters> UserParameters);

	UFUNCTION(BlueprintCallable)
	void setMatchParameters(int inMaxRounds = 3, float inEventDelay = 45.0f, FString inEraChosen = "Default",
		 EMatchTypes InMatchType = Classic);
protected:
	// Scores based on your ranking (IN ORDER)
	UPROPERTY(EditAnywhere)
	int ScoreValue[4] = {1000, 500, 300, 100};
	// Number of rounds per match
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Data")
	int MaxRounds = 5;
	// Delay between which events are summoned, if set to 0, events are disabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Data")
	float EventDelay = 55.0f;
	// Selected Era name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Data")
	FString EraName = "Default";
	// Current Match Type (Classic, Teams, etc...)
	UPROPERTY(EditAnywhere);
	TEnumAsByte<EMatchTypes> MatchType = Classic;
};