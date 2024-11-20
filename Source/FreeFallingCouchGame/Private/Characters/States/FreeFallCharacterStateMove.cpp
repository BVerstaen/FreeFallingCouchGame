// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateMove.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "Characters/States/FreeFallCharacterStateDive.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Settings/CharactersSettings.h"

EFreeFallCharacterStateID UFreeFallCharacterStateMove::GetStateID()
{
	return EFreeFallCharacterStateID::Move;
}

void UFreeFallCharacterStateMove::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	if(Character->GetLockControls())
	{
		Character->GetStateMachine()->ChangeState(EFreeFallCharacterStateID::Idle);
		return;
	}
	
	//Character->GetCharacterMovement()->MaxFlySpeed = StartMoveSpeed;
	Character->OnInputGrabEvent.AddDynamic(this, &UFreeFallCharacterStateMove::OnInputGrab);
	Character->OnInputUsePowerUpEvent.AddDynamic(this, &UFreeFallCharacterStateMove::OnInputUsePowerUp);
	Character->OnInputFastDiveEvent.AddDynamic(this, &UFreeFallCharacterStateMove::OnInputFastDive);
	
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
	Character->OnInputUsePowerUpEvent.RemoveDynamic(this, &UFreeFallCharacterStateMove::OnInputUsePowerUp);
	Character->OnInputFastDiveEvent.RemoveDynamic(this, &UFreeFallCharacterStateMove::OnInputFastDive);
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

	/*
	AccelerationAlpha += DeltaTime;
	Character->GetCharacterMovement()->MaxFlySpeed = FMath::Lerp(StartMoveSpeed,MaxMoveSpeed,FMath::Min(AccelerationAlpha/ReachMaxSpeedTime,1));
	*/
	const FVector2D InputMove = Character->GetInputMove();

	/*
	FVector MovementDirection = Character->GetVelocity().GetSafeNormal();
	

	/*
	//Set Orient Rotation To Movement
	if(Character->GetCharacterMovement()->bOrientRotationToMovement && Character->GrabbingState != EFreeFallCharacterGrabbingState::GrabHeavierObject)
	{
		//Get angle btw Character & movement direction
		float DotProduct = FVector::DotProduct(MovementDirection, CharacterDirection);
		
		//If Reached orientation Threshold in his grabbing state -> stop orientation and let yourself influenced
		if((DotProduct > OrientationThreshold && Character->OtherCharacterGrabbing)
			|| (DotProduct > GrabbedOrientationThreshold && Character->OtherCharacterGrabbedBy)
			|| Character->IsLookingToCloseToGrabber(GrabToCloseToGrabbedAngle))
		{
			Character->GetCharacterMovement()->bOrientRotationToMovement = false;
			GrabOldInputDirection = InputMove;
		}
	}
	else if(GrabOldInputDirection != InputMove)
	{
		//If you change direction -> Restore Orient Rotation Movement
		Character->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	*/
	
	//Set mesh movement
	FVector CharacterDirection = Character->GetActorForwardVector();
	FVector2D CharacterDirection2D = FVector2D( CharacterDirection.GetSafeNormal().X, CharacterDirection.GetSafeNormal().Y);
	float AngleDiff = FMath::Clamp(FVector2d::DotProduct(InputMove.GetSafeNormal(), CharacterDirection2D.GetSafeNormal()) , -1.0f , 1.0f);
	Character->InterpMeshPlayer(FRotator((AngleDiff >= 0 ? 1 : -1) * FMath::Lerp(Character->GetPlayerDefaultRotation().Pitch,MeshMovementRotationAngle, 1-FMath::Abs(AngleDiff)),
		Character->GetMesh()->GetRelativeRotation().Yaw, Character->GetPlayerDefaultRotation().Roll), DeltaTime, MeshMovementDampingSpeed);
	
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
		//Character->AddMovementInput(FVector::ForwardVector , InputMove.X);
		//Character->AddMovementInput(FVector::RightVector , InputMove.Y);
		
		if (FMath::Abs(InputMove.X) > CharactersSettings->InputMoveThreshold)
		{
			Character->AccelerationAlpha.X = FMath::Clamp(Character->AccelerationAlpha.X + InputMove.X * DeltaTime * AccelerationSpeed,
				-Character->MaxAccelerationValue, Character->MaxAccelerationValue);
		}
		if (FMath::Abs(InputMove.Y) > CharactersSettings->InputMoveThreshold)
		{
			Character->AccelerationAlpha.Y = FMath::Clamp(Character->AccelerationAlpha.Y + InputMove.Y * DeltaTime * AccelerationSpeed,
				-Character->MaxAccelerationValue, Character->MaxAccelerationValue);
		}

		if (Character->bShouldOrientToMovement)
		{
			FRotator RotationTarget = FVector(InputMove.X, InputMove.Y, 0).Rotation();
			Character->SetActorRotation(FMath::RInterpTo(Character->GetActorRotation(), RotationTarget, DeltaTime, 5));
		}

		GEngine->AddOnScreenDebugMessage(
			-1,
			DeltaTime,
			FColor::Cyan,
			"Move : " +Character->GetActorRotation().ToString()
			);
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

void UFreeFallCharacterStateMove::OnInputUsePowerUp()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::PowerUp);
}

void UFreeFallCharacterStateMove::OnInputFastDive()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::FastDive);
}
