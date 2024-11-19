// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateDive.h"

#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "Characters/States/FreeFallCharacterStateMove.h"
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

	UFreeFallCharacterStateMove* StateMove = Cast<UFreeFallCharacterStateMove>(StateMachine->GetState(EFreeFallCharacterStateID::Move));
	AccelerationSpeed = StateMove == nullptr? 0 : StateMove->AccelerationSpeed;

	//Exit State if is grabbed
	if(Character->OtherCharacterGrabbedBy)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Idle);
		return;
	}
	
	Character->GetMesh()->PlayAnimation(DiveAnimation, true);
	Character->OnInputFastDiveEvent.AddDynamic(this, &UFreeFallCharacterStateDive::OnInputFastDive);
	Character->bIsDiveForced = false;

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

	Character->bIsDiveForced = true;

	Character->OnInputFastDiveEvent.RemoveDynamic(this, &UFreeFallCharacterStateDive::OnInputFastDive);
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

	//Change Angle by rotation
	Character->InterpMeshPlayer(FRotator(Character->GetPlayerDefaultRotation().Pitch, Character->GetMesh()->GetRelativeRotation().Yaw, InputDive * MeshMovementRotationAngle), DeltaTime, MeshMovementDampingSpeed);

	
	if (FMath::Abs(InputDive) < CharactersSettings->InputMoveThreshold)
	{
		CurrentDivePhase = EDivePhase::DiveForcesApplying;
		/*
		if (!IsInCenterOfLayer())
		{
			//Dive Force Apply when Character is not in Center
			//ApplyDiveForce();
			Character->SetDiveMaterialColor();

		}*/
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
		FVector Direction = Character->GetActorLocation() - Character->GetCameraActor()->GetActorLocation();
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
				if (DiveLevelsActor->GetDiveLayersFromCoord(Character->GetActorLocation().Z) == EDiveLayersID::Middle)
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

/*
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
}*/

void UFreeFallCharacterStateDive::ApplyMoveInputs(float DeltaTime)
{
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue,TEXT("Can't apply move inputs"));
	FVector2D InputMove = Character->GetInputMove();
	if (FMath::Abs(InputMove.X) > CharactersSettings->InputMoveThreshold)
	{
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue,FString::SanitizeFloat(AccelerationSpeed));
		Character->AccelerationAlpha.X = FMath::Clamp(Character->AccelerationAlpha.X + InputMove.X * DeltaTime * AccelerationSpeed,
			-Character->MaxAccelerationValue, Character->MaxAccelerationValue);
	}
	if (FMath::Abs(InputMove.Y) > CharactersSettings->InputMoveThreshold)
	{
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Blue,TEXT("MoveY"));
		Character->AccelerationAlpha.Y = FMath::Clamp(Character->AccelerationAlpha.Y + InputMove.Y * DeltaTime * AccelerationSpeed,
			-Character->MaxAccelerationValue, Character->MaxAccelerationValue);
	}

	if (Character->bShouldOrientToMovement)
	{
		FRotator RotationTarget = FVector(InputMove.X, InputMove.Y, 0).Rotation();
		Character->SetActorRotation(FMath::RInterpTo(Character->GetActorRotation(), RotationTarget, DeltaTime, 5));
	}
}

/*
bool UFreeFallCharacterStateDive::IsInCenterOfLayer() const
{
	float ZPosition = Character->GetActorLocation().Z;
	EDiveLayersID CurrentLayer = Character->GetDiveLevelsActor()->GetDiveLayersFromCoord(ZPosition);

	return FMath::Abs(ZPosition - DiveLevelsActor->GetDiveBoundZCoord(CurrentLayer, EDiveLayerBoundsID::Middle)) < 0.1;
}*/

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

void UFreeFallCharacterStateDive::SetMoveStat(float Move_AccelerationSpeed)
{
	AccelerationSpeed = Move_AccelerationSpeed;
}

void UFreeFallCharacterStateDive::OnInputFastDive()
{
	StateMachine->ChangeState(EFreeFallCharacterStateID::FastDive);
}
