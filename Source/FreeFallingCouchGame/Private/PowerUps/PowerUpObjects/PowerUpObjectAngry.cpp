// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObjects/PowerUpObjectAngry.h"

#include "Characters/FreeFallCharacter.h"
#include "PowerUps/PowerUpsID.h"

void UPowerUpObjectAngry::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EffectClock+=DeltaTime;
	if (EffectClock >= Duration && !bIsActionFinished)
	{
		OwnerCharacter->PlayerMass = CharacterBaseMass;
		bIsActionFinished = true;
	}
}

void UPowerUpObjectAngry::Use()
{
	Super::Use();

	CharacterBaseMass = OwnerCharacter->PlayerMass;
	OwnerCharacter->PlayerMass = CharacterBaseMass * MassMultiplier;
}

EPowerUpsID UPowerUpObjectAngry::GetPowerUpID()
{
	return EPowerUpsID::Angry;
}
