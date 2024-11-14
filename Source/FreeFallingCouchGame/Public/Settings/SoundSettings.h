// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "Sound/SoundCue.h"
#include "SoundSettings.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSoundListData
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftObjectPtr<USoundCue>> Sounds;
};

UCLASS(Config=Game, DefaultConfig, meta= (DisplayName="Sound Settings"))
class FREEFALLINGCOUCHGAME_API USoundSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Sound Settings")
	TMap<FName, FSoundListData> SoundLists;

	UFUNCTION()
	void PlaySound(FName SoundName, AActor* ParentActor, bool bAttachToActor = true, float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f, float StartTime = 0.0f);

	UFUNCTION()
	void PlaySound2D(FName SoundName);
};