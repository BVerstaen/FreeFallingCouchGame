// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HapticsHandler.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, category = "Haptics")
class FREEFALLINGCOUCHGAME_API UHapticsHandler : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CallHapticsSingle"))
	void ReceiveCallHapticsCollision(APlayerController *Player);

	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CallHapticsAll"))
	void ReceiveCallHapticsAll();
};
