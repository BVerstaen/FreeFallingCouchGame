// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateMove.h"

#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetStringLibrary.h"
#include "Other/DiveLayersID.h"
#include "Other/DiveLevels.h"
#include "Settings/CharactersSettings.h"

EFreeFallCharacterStateID UFreeFallCharacterStateMove::GetStateID()
{
	return EFreeFallCharacterStateID::Move;
}

void UFreeFallCharacterStateMove::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->MaxFlySpeed = StartMoveSpeed;
	Character->OnInputGrabEvent.AddDynamic(this, &UFreeFallCharacterStateMove::OnInputGrab);

	//Set OrientRotation to movement (deactivated if is grabbed)
	Character->GetCharacterMovement()->bOrientRotationToMovement = true;
	if(Character->OtherCharacter != nullptr && !Character->bIsGrabbing)
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	
	if(PreviousStateID != EFreeFallCharacterStateID::Grab)
		AccelerationAlpha = 0;

	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	3.f,
	// 	FColor::Green,
	// 	TEXT("Enter State Move")
	// 	);
}

void UFreeFallCharacterStateMove::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	Character->OnInputGrabEvent.RemoveDynamic(this, &UFreeFallCharacterStateMove::OnInputGrab);
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	3.f,
	// 	FColor::Red,
	// 	TEXT("Exit State Move")
	// 	);
}

void UFreeFallCharacterStateMove::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	AccelerationAlpha += DeltaTime;
	Character->GetCharacterMovement()->MaxFlySpeed = FMath::Lerp(StartMoveSpeed,MaxMoveSpeed,FMath::Min(AccelerationAlpha/ReachMaxSpeedTime,1));

	const FVector2D InputMove = Character->GetInputMove();
	
	FVector MovementDirection = Character->GetVelocity().GetSafeNormal();
	FVector CharacterDirection = Character->GetActorForwardVector();

	//Set Orient Rotation To Movement
	if(Character->bIsGrabbing)
	{
		if(Character->GetCharacterMovement()->bOrientRotationToMovement)
		{
			//Get angle btw Character & movement direction
			float DotProduct = FVector::DotProduct(MovementDirection, CharacterDirection);
			if(DotProduct > OrientationThreshold)
			{
				Character->GetCharacterMovement()->bOrientRotationToMovement = false;
				OldInputDirection = InputMove;
			}
		}
		else if(OldInputDirection != InputMove)
		{
			//If you change direction -> Restore Orient Rotation Movement
			Character->GetCharacterMovement()->bOrientRotationToMovement = true;
		}		
	}

	
	//Change state if other input
	if (FMathf::Abs(Character->GetInputDive()) > CharactersSettings->InputDiveThreshold)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Dive);
		return;
	}
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

void UFreeFallCharacterStateMove::OnInputGrab()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::Grab);
}
