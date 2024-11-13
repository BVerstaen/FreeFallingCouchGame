// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUps/PowerUpObject.h"
#include "PowerUpObjectAngry.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FREEFALLINGCOUCHGAME_API UPowerUpObjectAngry : public UPowerUpObject
{
	GENERATED_BODY()

	virtual void Tick(float DeltaTime) override;

	virtual void Use() override;

	virtual EPowerUpsID GetPowerUpID() override;

private:
	UPROPERTY()
	float CharacterBaseMass;

	//Multiply base Player Mass with that number (more than 1 to make heavier)
	UPROPERTY(EditAnywhere)
	float MassMultiplier = 1.5f;
};
