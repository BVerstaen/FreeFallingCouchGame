﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObjects/PowerUpObjectAngry.h"

#include "Audio/SoundSubsystem.h"
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
		End();
	}
}

void UPowerUpObjectAngry::Use()
{
	Super::Use();

	USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_PLR_Piment_ST", OwnerCharacter, false);
	
	CharacterBaseMass = OwnerCharacter->PlayerMass;
	OwnerCharacter->PlayerMass = CharacterBaseMass * MassMultiplier;
}

EPowerUpsID UPowerUpObjectAngry::GetPowerUpID()
{
	return EPowerUpsID::Angry;
}
