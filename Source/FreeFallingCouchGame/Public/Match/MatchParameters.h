// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Arena/TrackerActor.h"
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

UCLASS(Blueprintable)
class FREEFALLINGCOUCHGAME_API UMatchParameters : public UObject
{
	GENERATED_BODY()
public:
	void Init(TSubclassOf<UMatchParameters> UserParameters);
	// Getters
	const int* getScoreValues() const;
	UFUNCTION(BlueprintCallable)
	int getMaxRounds() const;
	UFUNCTION(BlueprintCallable)
	float getTimerEventDelay() const {return EventDelay;}
	UFUNCTION(BlueprintCallable)
	float getRoundTimer() const {return RoundTimer;}
	UFUNCTION(BlueprintCallable)
	FString getEraChosen() const {return EraName;}
	UFUNCTION(BlueprintCallable)
	EMatchTypes getMatchType() const {return MatchType;}
	UFUNCTION(BlueprintCallable)
	ETrackingRewardCategory getTrackingRewardCategory() const { return TrackedValue;}
	
	// Setter
	//void setValues(TObjectPtr<UMatchParameters> UserParameters);

	UFUNCTION(BlueprintCallable)
	void setMatchParameters(int inMaxRounds = 3,float inRoundTimer = 55.0f, float inEventDelay = 45.0f, FString inEraChosen = "Default",
		 EMatchTypes InMatchType = Classic, ETrackingRewardCategory InTracker = LongestTimeWithParachute);
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Data")
	float RoundTimer = 55.0f;
	// Selected Era name
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Data")
	FString EraName = "Default";
	// Current Match Type (Classic, Teams, etc...)
	UPROPERTY(EditAnywhere);
	TEnumAsByte<EMatchTypes> MatchType = Classic;
	UPROPERTY(EditAnywhere);
	TEnumAsByte<ETrackingRewardCategory> TrackedValue = LongestTimeWithParachute;
};