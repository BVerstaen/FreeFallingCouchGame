// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUps/PowerUpObject.h"
#include "PowerUpObjectDash.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FREEFALLINGCOUCHGAME_API UPowerUpObjectDash : public UPowerUpObject
{
	GENERATED_BODY()
	
	virtual void Use() override;

	virtual EPowerUpsID GetPowerUpID() override;

protected:
	UPROPERTY(EditAnywhere)
	float DashForce = 10;
};
