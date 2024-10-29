// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FreeFallingCouchGame/Public/Characters/FreeFallCharacterInputData.h"
#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CharactersSettings.generated.h"
class UInputMappingContext;
//class UFreeFallCharacterInputData;
class UFreeFallCharacterState;
enum class EFreeFallCharacterStateID : uint8;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta= (DisplayName="Characters Settings"))
class FREEFALLINGCOUCHGAME_API UCharactersSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="States")
	TMap<EFreeFallCharacterStateID, TSubclassOf<UFreeFallCharacterState>> SmashCharacterStates;

	UPROPERTY(Config, EditAnywhere, Category="Ejection")
	float MarginHorizontalOffScreen;

	UPROPERTY(Config, EditAnywhere, Category="Ejection")
	float MarginVerticalOffScreen;
	
	UPROPERTY(Config, EditAnywhere, Category="Ejection")
	float PercentageCloseEdgeScreen;
	
	UPROPERTY(Config, EditAnywhere, Category="InputMove")
	float InputMoveThreshold;

	UPROPERTY(Config, EditAnywhere, Category="InputDive")
	float InputDiveThreshold;

	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	TSoftObjectPtr<UFreeFallCharacterInputData> InputData;

	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;
};
