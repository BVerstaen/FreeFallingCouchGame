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
