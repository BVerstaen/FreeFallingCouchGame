// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/MatchParameters.h"

void UMatchParameters::Init(TSubclassOf<UMatchParameters> UserParameters)
{
	auto refClass = UserParameters.GetDefaultObject();
	MaxRounds = refClass->getMaxRounds();
	EraName = refClass->getEraChosen();
	EventDelay = refClass->getTimerEventDelay();
	RoundTimer = refClass->getRoundTimer();
	ScoreValue = refClass->getScoreValues();
	TrackedValue = refClass->getTrackingRewardCategory();
	MatchType = refClass->getMatchType();
}

const TArray<int> UMatchParameters::getScoreValues() const
{
	return ScoreValue;
}

int UMatchParameters::getMaxRounds() const
{
	return MaxRounds;
}

void UMatchParameters::setMatchParameters(int inMaxRounds, float inEventDelay, float inRoundTimer, FString inEraChosen,
	EMatchTypes InMatchType, ETrackingRewardCategory InTracker)
{
	MaxRounds = inMaxRounds;
	EraName = inEraChosen;
	EventDelay = inEventDelay;
	MatchType = InMatchType;
	RoundTimer = inRoundTimer;
	TrackedValue = InTracker;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Received Data !");
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(
	TEXT("Selected Rounds : %i \n  Event delay: %f"), 
	inMaxRounds));
}
