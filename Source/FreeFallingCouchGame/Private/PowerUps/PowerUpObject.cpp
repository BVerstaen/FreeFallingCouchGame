// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObject.h"

UPowerUpObject::UPowerUpObject()
{
}

void UPowerUpObject::Use()
{
}

void UPowerUpObject::SetupCharacter(AFreeFallCharacter* Character)
{
	OwnerCharacter = Character;
}

void UPowerUpObject::PrepareForDestruction()
{
}

