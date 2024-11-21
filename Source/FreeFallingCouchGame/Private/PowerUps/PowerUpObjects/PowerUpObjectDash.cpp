// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpObjects/PowerUpObjectDash.h"

#include "Characters/FreeFallCharacter.h"
#include "PowerUps/PowerUpsID.h"

void UPowerUpObjectDash::Use()
{
	Super::Use();

	OwnerCharacter->LaunchCharacter(OwnerCharacter->GetActorForwardVector() * DashForce, false, true);

	bIsActionFinished = true;
}

EPowerUpsID UPowerUpObjectDash::GetPowerUpID()
{
	return EPowerUpsID::Dash;
}
