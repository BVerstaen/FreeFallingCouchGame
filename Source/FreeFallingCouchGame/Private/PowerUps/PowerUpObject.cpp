// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObject.h"

#include "Characters/FreeFallCharacter.h"
#include "PowerUps/PowerUpsID.h"

UPowerUpObject::UPowerUpObject()
{
}

void UPowerUpObject::Use()
{
	OwnerCharacter->PlayerPowerUpEvent(GetPowerUpID());
	bHasBeenUsed = true;
}	

void UPowerUpObject::End()
{
	OwnerCharacter->PlayerPowerUpEventEnd(GetPowerUpID());
}

void UPowerUpObject::Tick(float DeltaTime)
{
	if (GetPowerUpID() == EPowerUpsID::None)
	{
		GEngine->AddOnScreenDebugMessage(-1,DeltaTime,FColor::Turquoise,TEXT("Tick PowerUp None"));
		EffectClock+=DeltaTime;
		if (EffectClock >= Duration) bIsActionFinished = true;
	}
}

void UPowerUpObject::ResetEffectClock()
{
	EffectClock = 0.f;
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

