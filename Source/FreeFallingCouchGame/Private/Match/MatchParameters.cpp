// Fill out your copyright notice in the Description page of Project Settings.


#include "Match/MatchParameters.h"

const int* UMatchParameters::getScoreValues() const
{
	return ScoreValue;
}

int UMatchParameters::getMaxRounds() const
{
	return MaxRounds;
}

void UMatchParameters::setValues(const UMatchParameters& UserParameters)
{
	memcpy(ScoreValue,UserParameters.getScoreValues() , sizeof(int));
	MaxRounds = UserParameters.getMaxRounds();
	MatchType  = UserParameters.MatchType;
}