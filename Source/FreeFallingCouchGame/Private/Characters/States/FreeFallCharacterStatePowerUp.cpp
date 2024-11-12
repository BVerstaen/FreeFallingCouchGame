﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStatePowerUp.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "PowerUps/PowerUpObject.h"

void UFreeFallCharacterStatePowerUp::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	if (Character->CurrentPowerUp ==nullptr || Character->CurrentPowerUp->hasBeenUsed)
	{
		ExitStatePowerUp();
		return;
	}
	Character->CurrentPowerUp->Use();
	Character->OnUsePowerUp.Broadcast(Character);
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Emerald,"Using PowerUp");
	ExitStatePowerUp();
}

void UFreeFallCharacterStatePowerUp::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}

EFreeFallCharacterStateID UFreeFallCharacterStatePowerUp::GetStateID()
{
	return EFreeFallCharacterStateID::PowerUp;
}

void UFreeFallCharacterStatePowerUp::ExitStatePowerUp()
{
	if (FMathf::Abs(Character->GetInputDive()) > CharactersSettings->InputDiveThreshold)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Dive);
		return;
	}
	if (FMath::Abs(Character->GetInputMove().X) > CharactersSettings->InputMoveThreshold ||
		FMath::Abs(Character->GetInputMove().Y) > CharactersSettings->InputMoveThreshold)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Move);
	}
	else
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Idle);
	}
}