// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PowerUps/PowerUpObject.h"
#include "PowerUpObjectSmaller.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FREEFALLINGCOUCHGAME_API UPowerUpObjectSmaller : public UPowerUpObject
{
	GENERATED_BODY()

	virtual void Tick(float DeltaTime) override;

	virtual void Use() override;

	virtual EPowerUpsID GetPowerUpID() override;

private:
	UPROPERTY()
	FVector CharacterBaseScale;

	//Multiply base scale with that number (between 0 and 1)
	UPROPERTY(EditAnywhere)
	float ScaleMultiplier = 0.5f;
};
