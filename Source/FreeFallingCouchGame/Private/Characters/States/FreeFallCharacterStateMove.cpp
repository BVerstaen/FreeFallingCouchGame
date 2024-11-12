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

	Character->GetCharacterMovement()->MaxFlySpeed = StartMoveSpeed;
	Character->OnInputGrabEvent.AddDynamic(this, &UFreeFallCharacterStateMove::OnInputGrab);
	Character->OnInputUsePowerUpEvent.AddDynamic(this, &UFreeFallCharacterStateMove::OnInputUsePowerUp);

	//Set OrientRotation to movement (deactivated if grab a heavier object)
	Character->GetCharacterMovement()->bOrientRotationToMovement = Character->GrabbingState != EFreeFallCharacterGrabbingState::GrabHeavierObject;
	
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

	if (NextStateID == EFreeFallCharacterStateID::Dive)
	{
		Cast<UFreeFallCharacterStateDive>(Character->GetStateMachine()->GetState(NextStateID))->SetMoveStats(MaxMoveSpeed, StartMoveSpeed, ReachMaxSpeedTime, OrientationThreshold, &AccelerationAlpha);
	}

	Character->OnInputGrabEvent.RemoveDynamic(this, &UFreeFallCharacterStateMove::OnInputGrab);
	Character->OnInputUsePowerUpEvent.RemoveDynamic(this, &UFreeFallCharacterStateMove::OnInputUsePowerUp);
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

	FVector2D CharacterDirection2D = FVector2D(CharacterDirection.GetSafeNormal().X, CharacterDirection.GetSafeNormal().Y);
	float AngleDiff = FVector2d::DotProduct(InputMove.GetSafeNormal(), CharacterDirection2D.GetSafeNormal());
	AngleDiff = FMath::Clamp(AngleDiff, -1, 1);
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Black, FString::SanitizeFloat((AngleDiff > 0 ? 1 : -1)));
	
	//Set mesh movement
	Character->InterpMeshPlayer(FRotator((AngleDiff > 0 ? 1 : -1) * FMath::Lerp(Character->GetPlayerDefaultRotation().Pitch,MeshMovementRotationAngle, 1-FMath::Abs(AngleDiff)),
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

void UFreeFallCharacterStateMove::OnInputUsePowerUp()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::PowerUp);
}
