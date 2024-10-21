// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateMove.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/CharactersSettings.h"

EFreeFallCharacterStateID UFreeFallCharacterStateMove::GetStateID()
{
	return EFreeFallCharacterStateID::Move;
}

void UFreeFallCharacterStateMove::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->MaxWalkSpeed = StartMoveSpeed;

	AccelerationAlpha = 0;

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

	AccelerationAlpha += DeltaTime;
	Character->GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(StartMoveSpeed,MaxMoveSpeed,FMath::Min(AccelerationAlpha/ReachMaxSpeedTime,1));

	const FVector2D InputMove = Character->GetInputMove();
	
	if (FMath::Abs(InputMove.Y) < CharactersSettings->InputMoveThreshold && FMath::Abs(InputMove.X) < CharactersSettings->InputMoveThreshold)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Idle);
	}
	else
	{
		//Character->GetCharacterMovement()->AddForce(FVector::ForwardVector * FVector(InputMove.X, InputMove.Y, 0) );
		Character->AddMovementInput(FVector::ForwardVector , InputMove.X);
		Character->AddMovementInput(FVector::RightVector , InputMove.Y);
	}
	GEngine->AddOnScreenDebugMessage(
		-1,
		DeltaTime,
		FColor::Cyan,
		TEXT("Tick State Move")
		);
}
