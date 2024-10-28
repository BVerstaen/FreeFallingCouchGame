// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/MatchParameters.h"

void UMatchParameters::Init(TSubclassOf<UMatchParameters> UserParameters)
{
	MaxRounds = UserParameters.GetDefaultObject()->getMaxRounds();
	EraName = UserParameters.GetDefaultObject()->getEraChosen();
	EventDelay = UserParameters.GetDefaultObject()->getTimerDelay();
	memcpy(ScoreValue, UserParameters.GetDefaultObject()->getScoreValues(), sizeof(int*));
}

const int* UMatchParameters::getScoreValues() const
{
	return ScoreValue;
}

int UMatchParameters::getMaxRounds() const
{
	return MaxRounds;
}

void UMatchParameters::setValues(TObjectPtr<UMatchParameters> UserParameters)
{
	if(IsValid(UserParameters))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Valid data");
		memcpy(ScoreValue,UserParameters->getScoreValues() , sizeof(int));
		MaxRounds = UserParameters->getMaxRounds();
		//MatchType  = UserParameters.MatchType;
	} else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Data not valid");
	}
}

void UMatchParameters::setMatchParameters(int inMaxRounds, float inEventDelay, FString inEraChosen,
	EMatchTypes InMatchType)
{
	MaxRounds = inMaxRounds;
	EraName = inEraChosen;
	EventDelay = inEventDelay;
	MatchType = InMatchType;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Received Data !");
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(TEXT("Selected Rounds : %i"), inMaxRounds));
}
