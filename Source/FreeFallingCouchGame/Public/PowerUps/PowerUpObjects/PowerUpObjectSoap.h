// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUps/PowerUpObject.h"
#include "PowerUpObjectSoap.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FREEFALLINGCOUCHGAME_API UPowerUpObjectSoap : public UPowerUpObject
{
	GENERATED_BODY()
	
	virtual void Tick(float DeltaTime) override;

	virtual void Use() override;

	virtual EPowerUpsID GetPowerUpID() override;
};
