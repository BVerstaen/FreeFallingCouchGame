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
	UAudioComponent* PlaySound(FName SoundName, const AActor* ParentActor, bool bAttachToActor = true, float PitchMultiplier = 1.0f, float StartTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Sound")
	void Play2DSound(FName SoundName);

	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayMusic(FName MusicName, float PitchMultiplier = 1.0f, float StartTime = 0.0f);

	UFUNCTION(BlueprintCallable, Category = "Music")
	void UpdateMusicVolume();
	
	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic();

	UFUNCTION(BlueprintPure, Category = "Music")
	UAudioComponent* GetAudioComponent();
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sound")
	float SoundVolume = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Sound")
	float MusicVolume = 1.0f;

	UPROPERTY()
	UAudioComponent* MusicComponent;
};
