// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObjects/PowerUpObjectSmaller.h"

#include "Audio/SoundSubsystem.h"
#include "Characters/FreeFallCharacter.h"
#include "PowerUps/PowerUpsID.h"

void UPowerUpObjectSmaller::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EffectClock+=DeltaTime;
	if (EffectClock >= Duration && !bIsActionFinished)
	{
		OwnerCharacter->DiveScaleFactor = CharacterBaseScaleMultiplier;
		bIsActionFinished = true;
		End();
	}
}

void UPowerUpObjectSmaller::Use()
{
	Super::Use();

	USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_PLR_Potiondrink_ST", OwnerCharacter, false);
	SoundSubsystem->PlaySound("SFX_PLR_becomelittle_ST", OwnerCharacter, false);
	
	CharacterBaseScaleMultiplier = OwnerCharacter->DiveScaleFactor;
	OwnerCharacter->DiveScaleFactor = ScaleMultiplier;
}

EPowerUpsID UPowerUpObjectSmaller::GetPowerUpID()
{
	return EPowerUpsID::Smaller;
}
