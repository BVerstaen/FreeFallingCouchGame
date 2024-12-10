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
	UFUNCTION()
	 void CallHapticsSingle(APlayerController *Player);

	UFUNCTION()
	void CallHapticsSingleDive(APlayerController *Player, bool bIsDivingDown);
	
	UFUNCTION()
	 void CallHapticsMulti();

	// Do not use this outside of blueprints
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CallHapticsSingle"))
	void ReceiveCallHapticsCollision(APlayerController *Player);

	// Do not use this outside of blueprints
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CallHapticsAll"))
	void ReceiveCallHapticsAll();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "CallHapticsSingleDive"))
	void ReceiveCallHapticsDive(APlayerController *Player, bool bIsDivingDown);
};

