// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MapSettings.generated.h"

class AFreeFallCharacter;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta= (DisplayName="Map Settings"))
class FREEFALLINGCOUCHGAME_API UMapSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP0;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP1;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP2;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP3;
};
