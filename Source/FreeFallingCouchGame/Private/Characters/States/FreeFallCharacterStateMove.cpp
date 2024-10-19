// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateMove.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"

EFreeFallCharacterStateID UFreeFallCharacterStateMove::GetStateID()
{
	return EFreeFallCharacterStateID::Move;
}

void UFreeFallCharacterStateMove::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Green,
		TEXT("Enter State Move")
		);
}

void UFreeFallCharacterStateMove::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Exit State Move")
		);
}

void UFreeFallCharacterStateMove::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	Character->AddMovementInput(FVector::ForwardVector, 1);

	GEngine->AddOnScreenDebugMessage(
		-1,
		DeltaTime,
		FColor::Cyan,
		TEXT("Tick State Move")
		);
}
