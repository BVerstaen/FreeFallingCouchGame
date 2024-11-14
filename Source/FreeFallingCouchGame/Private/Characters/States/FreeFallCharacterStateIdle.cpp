// Fill out your copyright notice in the Description page of Project Settings.


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

	Character->OnInputGrabEvent.AddDynamic(this, &UFreeFallCharacterStateIdle::OnInputGrab);
	Character->OnInputUsePowerUpEvent.AddDynamic(this, &UFreeFallCharacterStateIdle::OnInputUsePowerUp);
	Character->OnInputFastDiveEvent.AddDynamic(this, &UFreeFallCharacterStateIdle::OnInputFastDive);
	
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	3.f,
	// 	FColor::Green,
	// 	TEXT("Enter State Idle")
	// 	);
}

void UFreeFallCharacterStateIdle::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	Character->OnInputGrabEvent.RemoveDynamic(this, &UFreeFallCharacterStateIdle::OnInputGrab);
	Character->OnInputUsePowerUpEvent.RemoveDynamic(this, &UFreeFallCharacterStateIdle::OnInputUsePowerUp);
	Character->OnInputFastDiveEvent.RemoveDynamic(this, &UFreeFallCharacterStateIdle::OnInputFastDive);

	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	3.f,
	// 	FColor::Red,
	// 	TEXT("Exit State Idle")
	// 	);
}

void UFreeFallCharacterStateIdle::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	//Change State on input
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

	//Reset mesh rotation
	Character->InterpMeshPlayer(Character->GetPlayerDefaultRotation(), DeltaTime, MeshMovementDampingSpeed);
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		DeltaTime,
		FColor::Cyan,
		TEXT("Tick State Idle")
		);
}

void UFreeFallCharacterStateIdle::OnInputGrab()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::Grab);
}

void UFreeFallCharacterStateIdle::OnInputUsePowerUp()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::PowerUp);
}

void UFreeFallCharacterStateIdle::OnInputFastDive()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::FastDive);
}
