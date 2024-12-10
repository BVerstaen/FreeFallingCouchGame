// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HapticsStatics.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UHapticsStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category="Haptics", meta=(WorldContext="WorldContextObject"))
	static void CallHapticsCollision(UObject* WorldContextObject, APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable, Category="Haptics", meta=(WorldContext="WorldContextObject"))
	static void CallHapticsDive(UObject* WorldContextObject, APlayerController* PlayerController, bool bIsDivingDown);
	
	UFUNCTION(BlueprintCallable, Category="Haptics", meta=(WorldContext="WorldContextObject"))
	static void CallHapticsAll(UObject* WorldContextObject);

};
