// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FreeFallCharacterState.h"

#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"

UFreeFallCharacterState::UFreeFallCharacterState()
{
	CharactersSettings = GetDefault<UCharactersSettings>();
}

EFreeFallCharacterStateID UFreeFallCharacterState::GetStateID()
{
	return EFreeFallCharacterStateID::None;
}

void UFreeFallCharacterState::StateInit(UFreeFallCharacterStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;
	Character = InStateMachine->GetCharacter();
}

void UFreeFallCharacterState::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
}

void UFreeFallCharacterState::StateTick(float DeltaTime)
{
}

void UFreeFallCharacterState::StateExit(EFreeFallCharacterStateID NextStateID)
{
}
