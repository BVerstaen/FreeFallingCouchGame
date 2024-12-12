// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObjects/PowerUpObjectSoap.h"

#include "Audio/SoundSubsystem.h"
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
		End();
	}
}

void UPowerUpObjectSoap::Use()
{
	Super::Use();

	USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_GPE_Savon_ST", OwnerCharacter, false);
	
	OwnerCharacter->bIsGrabbable = false;
	
}

EPowerUpsID UPowerUpObjectSoap::GetPowerUpID()
{
	return EPowerUpsID::Soap;
}
