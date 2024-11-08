// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObject.h"

#include "PowerUps/PowerUpsID.h"

UPowerUpObject::UPowerUpObject()
{
}

void UPowerUpObject::Use()
{
	hasBeenUsed = true;
}

void UPowerUpObject::SetupCharacter(AFreeFallCharacter* Character)
{
	OwnerCharacter = Character;
}

void UPowerUpObject::PrepareForDestruction()
{
}

EPowerUpsID UPowerUpObject::GetPowerUpID()
{
	return EPowerUpsID::None;
}

