// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundCounterWidget.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API URoundCounterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishCounter);
	FOnFinishCounter OnFinishCounter;
	
	UFUNCTION(BlueprintCallable)
	void CallOnFinishCounter();
};
