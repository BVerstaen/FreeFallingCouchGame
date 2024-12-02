// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HapticsHandler.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "HapticsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UHapticsSubsystem : public UGameInstanceSubsystem
{
public:
	UHapticsHandler* GetHaptics() const;
	void InitHaptics(TSubclassOf<UHapticsHandler> Template);
protected:
	UPROPERTY()
	UHapticsHandler *InstanceHaptics;
	
	GENERATED_BODY()
};
