// Fill out your copyright notice in the Description page of Project Settings.

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
};
