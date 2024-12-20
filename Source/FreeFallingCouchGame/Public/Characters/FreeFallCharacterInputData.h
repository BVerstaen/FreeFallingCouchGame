﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FreeFallCharacterInputData.generated.h"

class UInputAction;
/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterInputData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionMove;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionDive;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionFastDive;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionGrab;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionDeGrab;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionUsePowerUp;
};
