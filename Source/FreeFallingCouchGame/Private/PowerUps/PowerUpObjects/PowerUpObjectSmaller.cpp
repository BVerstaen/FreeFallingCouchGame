// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObjects/PowerUpObjectSmaller.h"

#include "Characters/FreeFallCharacter.h"
#include "PowerUps/PowerUpsID.h"

void UPowerUpObjectSmaller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EffectClock+=DeltaTime;
	if (EffectClock >= Duration && !bIsActionFinished)
	{
		OwnerCharacter->SetActorScale3D(CharacterBaseScale);
		bIsActionFinished = true;
	}
}

void UPowerUpObjectSmaller::Use()
{
	Super::Use();

	CharacterBaseScale = OwnerCharacter->GetActorScale();
	OwnerCharacter->SetActorScale3D(CharacterBaseScale * ScaleMultiplier);
}

EPowerUpsID UPowerUpObjectSmaller::GetPowerUpID()
{
	return EPowerUpsID::Smaller;
}
