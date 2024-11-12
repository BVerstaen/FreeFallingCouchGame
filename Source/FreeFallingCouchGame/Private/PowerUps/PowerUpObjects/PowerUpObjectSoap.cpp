// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObjects/PowerUpObjectSoap.h"

#include "Characters/FreeFallCharacter.h"
#include "PowerUps/PowerUpsID.h"

void UPowerUpObjectSoap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EffectClock+=DeltaTime;
	if (EffectClock >= Duration && !bIsActionFinished)
	{
		OwnerCharacter->bIsGrabbable = true;
		bIsActionFinished = true;
	}
}

void UPowerUpObjectSoap::Use()
{
	Super::Use();

	OwnerCharacter->bIsGrabbable = false;
	
}

EPowerUpsID UPowerUpObjectSoap::GetPowerUpID()
{
	return EPowerUpsID::Soap;
}
