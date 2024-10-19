﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateIdle.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "Settings/CharactersSettings.h"

EFreeFallCharacterStateID UFreeFallCharacterStateIdle::GetStateID()
{
	return EFreeFallCharacterStateID::Idle;
}

void UFreeFallCharacterStateIdle::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Green,
		TEXT("Enter State Idle")
		);
}

void UFreeFallCharacterStateIdle::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Exit State Idle")
		);
}

void UFreeFallCharacterStateIdle::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	/*if (FMath::Abs(Character->GetInputMove()) > CharactersSettings->InputMoveXThreshold)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Move);
	}*/
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		DeltaTime,
		FColor::Cyan,
		TEXT("Tick State Idle")
		);
}
