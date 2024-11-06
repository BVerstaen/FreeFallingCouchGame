﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MapSettings.generated.h"

class ALevelInstance;
class AParachute;
class AFreeFallCharacter;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta= (DisplayName="Map Settings"))
class FREEFALLINGCOUCHGAME_API UMapSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Map")
	TArray<TSoftObjectPtr<UWorld>> PlayerStartsLevels;

	UPROPERTY(Config, EditAnywhere, Category = "Map")
	FVector PlayerStartSubLevelLocation;
	
	//TEMPORAIRE !! A Remplacer par le nombre de mannette connecté dans Character Selection screen
	UPROPERTY(Config, EditAnywhere, Category = "Map")
	int NumberOfPlayers;
	
	
	UPROPERTY(Config, EditAnywhere, Category="Parachute")
	TSubclassOf<AParachute> ParachuteSubclass;
	
	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP0;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP1;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP2;

	UPROPERTY(Config, EditAnywhere, Category = "Characters")
	TSubclassOf<AFreeFallCharacter> CharacterClassP3;

	UPROPERTY(Config, EditAnywhere, Category="Diving")
	float DiveLayerThreshold;
};
