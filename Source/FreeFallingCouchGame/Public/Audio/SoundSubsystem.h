// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Settings/SoundSettings.h"
#include "SoundSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API USoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Sound")
	UAudioComponent* PlaySound(FName SoundName, const AActor* ParentActor, bool bAttachToActor = true, float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f, float StartTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void Play2DSound(FName SoundName);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void PlayMusic(FName MusicName, bool bLoop = true, float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f, float StartTime = 0.0f);

};
