// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateDive.h"

#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Other/DiveLayersID.h"
#include "Other/DiveLevels.h"


EFreeFallCharacterStateID UFreeFallCharacterStateDive::GetStateID()
{
	return EFreeFallCharacterStateID::Dive;
}

void UFreeFallCharacterStateDive::StateInit(UFreeFallCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);

	DiveLevelsActor = Character->GetDiveLevelsActor();
}

void UFreeFallCharacterStateDive::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	//Not crash if DiveLevelsActor is not set in scene
	if (DiveLevelsActor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "DiveLevelsActor is not set on Scene, Actor won't dive");
		if (FMath::Abs(Character->GetInputMove().X) > CharactersSettings->InputMoveThreshold ||
				FMath::Abs(Character->GetInputMove().Y) > CharactersSettings->InputMoveThreshold)
		{
			StateMachine->ChangeState(EFreeFallCharacterStateID::Move);
		}
		else
		{
			StateMachine->ChangeState(EFreeFallCharacterStateID::Idle);
		}
		return;
	}
	
	CheckTargetedLayer();
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		"Entering StateDive");	
}

void UFreeFallCharacterStateDive::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}

void UFreeFallCharacterStateDive::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if (DiveLevelsActor == nullptr) return;

	float InputDive = Character->GetInputDive();

	ApplyMoveInputs(DeltaTime);

	GEngine->AddOnScreenDebugMessage(
		-1,
		DeltaTime,
		FColor::Red,
		TEXT("InputDive = " + FString::SanitizeFloat(InputDive) +
			", ActorLocation : " +FString::SanitizeFloat(Character->GetActorLocation().Z)
			));
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		DeltaTime,
		FColor::Turquoise,
		TEXT("TargetLayer = " + GetLayerName(TargetLayer))
		);
	
	
	if (FMath::Abs(InputDive) < CharactersSettings->InputMoveThreshold)
	{
		CurrentDivePhase = EDivePhase::DiveForcesApplying;
		if (!IsInCenterOfLayer())
		{
			//Dive Force Apply when Character is not in Center
			ApplyDiveForce();
			Character->SetDiveMaterialColor();

		}
		else
		{
			//If Character is in Center, ChangeState to Idle or Move;
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
	}
	else
	{
		if (CurrentDivePhase == EDivePhase::DiveForcesApplying) CheckTargetedLayer();
		CrossLayerClock += DeltaTime;
		GEngine->AddOnScreenDebugMessage(
			-1,
			DeltaTime,
			FColor::Blue,
			CurrentDivePhase == EDivePhase::DiveForcesApplying ? "DiveForcesApplying" :
			CurrentDivePhase == EDivePhase::ChangingLayer ? "ChangingLayer" : "CrossingLayer");
		FVector Direction = Character->GetCameraActor()->GetActorForwardVector();
		Direction.Normalize();
		if (DiveLevelsActor->GetDiveBoundZCoord(TargetLayer, EDiveLayerBoundsID::Up) - DiveLayerThreshold > Character->GetActorLocation().Z && InputDive < 0)
		{
			Character->AddMovementInput(Direction, -1);
		}
		else if (InputDive < 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Can't go up"));
			FVector Velocity = Character->GetCharacterMovement()->Velocity;
			if (Velocity.Z > 1) Character->GetCharacterMovement()->Velocity += Velocity.Z * Direction;
		}
		
		if (DiveLevelsActor->GetDiveBoundZCoord(TargetLayer, EDiveLayerBoundsID::Down) + DiveLayerThreshold < Character->GetActorLocation().Z && InputDive > 0)
		{
			Character->AddMovementInput(Direction,  1);
		}
		else if (InputDive > 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Can't go down"));
			FVector Velocity = Character->GetCharacterMovement()->Velocity;
			if (Velocity.Z < -1) Character->GetCharacterMovement()->Velocity += Velocity.Z * Direction;
		}
		if (DiveLevelsActor->GetDiveLayersFromCoord(Character->GetActorLocation().Z) == TargetLayer)
		{
			if (CurrentDivePhase == EDivePhase::ChangingLayer)
			{
				CurrentDivePhase = EDivePhase::CrossingLayer;
				FVector Velocity = Character->GetCharacterMovement()->Velocity;
				if (Velocity.Z < -1) Character->GetCharacterMovement()->Velocity += Velocity.Z * Direction;
				Character->GetCharacterMovement()->MaxFlySpeed = DiveLevelsActor->GetDiveSize() / CrossLayerCooldown;
				CrossLayerClock = 0.f;
			}
		}
		if (CurrentDivePhase != EDivePhase::ChangingLayer && CrossLayerClock > CrossLayerCooldown) CheckTargetedLayer();
		
		Character->SetDiveMaterialColor();
	}
}

void UFreeFallCharacterStateDive::CheckTargetedLayer()
{
	CurrentDivePhase = EDivePhase::ChangingLayer;
	float ZPosition = Character->GetActorLocation().Z;
	EDiveLayersID CurrentLayer = DiveLevelsActor->GetDiveLayersFromCoord(ZPosition);
	if (Character->GetInputDive() < 0)
	{
		switch (CurrentLayer)
		{
			case EDiveLayersID::Top:
				TargetLayer = CurrentLayer;
				break;
			case EDiveLayersID::Middle:
				TargetLayer = EDiveLayersID::Top;
			break;
			case EDiveLayersID::Bottom:
				TargetLayer = EDiveLayersID::Middle;
			break;
			
			default:
				return;
		}
	}
	else
	{
		switch (CurrentLayer)
		{
			case EDiveLayersID::Top:
				TargetLayer = EDiveLayersID::Middle;
				break;
			case EDiveLayersID::Middle:
				TargetLayer = EDiveLayersID::Bottom;
				break;
			case EDiveLayersID::Bottom:
				TargetLayer = CurrentLayer;
				break;
			default:
				return;
		}
	}

	Character->GetCharacterMovement()->MaxFlySpeed = DiveSpeed;
}


void UFreeFallCharacterStateDive::ApplyDiveForce()
{
	float ZPosition = Character->GetActorLocation().Z;
	EDiveLayersID CurrentLayer = DiveLevelsActor->GetDiveLayersFromCoord(ZPosition);
	float ZPosDifference = ZPosition - DiveLevelsActor->GetDiveBoundZCoord(CurrentLayer, EDiveLayerBoundsID::Middle);
	Character->GetCharacterMovement()->MaxFlySpeed = Character->GetDiveLayerForceStrength();
	if (FMath::Abs(ZPosDifference) > CharactersSettings->DiveLayerThreshold)
	{
		FVector Direction = Character->GetCameraActor()->GetActorForwardVector();
		Direction.Normalize();
		Character->AddMovementInput(Direction, ZPosDifference < 0 ? -1 : 1);
	}
	GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Red, "DiveForces");
}

void UFreeFallCharacterStateDive::ApplyMoveInputs(float DeltaTime)
{
	if (AccelerationAlpha == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, "Acceleration is not set");
		return;
	}
	*AccelerationAlpha += DeltaTime;
	Character->GetCharacterMovement()->MaxFlySpeed = FMath::Lerp(StartMoveSpeed,MaxMoveSpeed,FMath::Min(*AccelerationAlpha / ReachMaxSpeedTime,1));

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

	
	
	if (FMath::Abs(InputMove.Y) > CharactersSettings->InputMoveThreshold || FMath::Abs(InputMove.X) > CharactersSettings->InputMoveThreshold)
	{
		//Character->GetCharacterMovement()->AddForce(FVector::ForwardVector * FVector(InputMove.X, InputMove.Y, 0) );
		Character->AddMovementInput(FVector::ForwardVector , InputMove.X);
		Character->AddMovementInput(FVector::RightVector , InputMove.Y);
	}
	else
	{
		*AccelerationAlpha = 0.f;
	}
}

bool UFreeFallCharacterStateDive::IsInCenterOfLayer() const
{
	float ZPosition = Character->GetActorLocation().Z;
	EDiveLayersID CurrentLayer = Character->GetDiveLevelsActor()->GetDiveLayersFromCoord(ZPosition);

	return FMath::Abs(ZPosition - DiveLevelsActor->GetDiveBoundZCoord(CurrentLayer, EDiveLayerBoundsID::Middle))
	< CharactersSettings->DiveLayerThreshold;
}

FString UFreeFallCharacterStateDive::GetLayerName(EDiveLayersID LayerID) const
{
	switch (LayerID)
	{
		case EDiveLayersID::Top: return FString("Top");
		case EDiveLayersID::Middle: return FString("Middle");
		case EDiveLayersID::Bottom: return FString("Bottom");
		default: return FString("Unknown");
	}
}

void UFreeFallCharacterStateDive::SetMoveStats(float Move_MaxMoveSpeed, float Move_StartMoveSpeed,
	float Move_ReachMaxSpeedTime, float Move_OrientationThreshold, float* Move_AccelerationAlpha)
{
	MaxMoveSpeed = Move_MaxMoveSpeed;
	StartMoveSpeed = Move_StartMoveSpeed;
	ReachMaxSpeedTime = Move_ReachMaxSpeedTime;
	OrientationThreshold = Move_OrientationThreshold;
	AccelerationAlpha = Move_AccelerationAlpha;
}
