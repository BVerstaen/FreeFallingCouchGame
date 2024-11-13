// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateFastDive.h"

#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Other/DiveLevels.h"


EFreeFallCharacterStateID UFreeFallCharacterStateFastDive::GetStateID()
{
	return EFreeFallCharacterStateID::FastDive;
}

void UFreeFallCharacterStateFastDive::StateInit(UFreeFallCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);

	DiveLevelsActor = Character->GetDiveLevelsActor();
}

void UFreeFallCharacterStateFastDive::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

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
		"Entering StateFastDive");	
}

void UFreeFallCharacterStateFastDive::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	Character->bIsDiveForced = true;
}

void UFreeFallCharacterStateFastDive::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Cyan, "StateFastDive Tick");
	
	FVector Direction = Character->GetActorLocation() - Character->GetCameraActor()->GetActorLocation();
	Direction.Normalize();

	GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Purple, "CharZLoc : " + FString::SanitizeFloat(Character->GetActorLocation().Z));
	GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Purple, "ZTarget : " + FString::SanitizeFloat(TargetLayerZCenter));


	if (Character->GetActorLocation().Z > TargetLayerZCenter && DirectionScaleValue == -1)
	{
		FVector Velocity = Character->GetCharacterMovement()->Velocity;
		Character->GetCharacterMovement()->Velocity += Velocity.Z * Direction;
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple, "Velocity with DSV-1 : " + FString::SanitizeFloat(Velocity.Z));
		ExitStateFastDive();
		return;
	}
	if (Character->GetActorLocation().Z < TargetLayerZCenter && DirectionScaleValue == 1)
	{
		FVector Velocity = Character->GetCharacterMovement()->Velocity;
		Character->GetCharacterMovement()->Velocity += Velocity.Z * Direction;
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple, "Velocity with DSV1 : " + FString::SanitizeFloat(Velocity.Z));
		ExitStateFastDive();
		return;
	}
	Character->AddMovementInput(Direction, DirectionScaleValue);
}

void UFreeFallCharacterStateFastDive::CheckTargetedLayer()
{
	const float ZPosition = Character->GetActorLocation().Z;
	const EDiveLayersID CurrentLayer = DiveLevelsActor->GetDiveLayersFromCoord(ZPosition);
	if (Character->GetInputFastDive() < 0)
	{
		switch (CurrentLayer)
		{
		case EDiveLayersID::Top:
			TargetLayer = CurrentLayer;
			//Exit StateFastDive;
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
	if (TargetLayer == CurrentLayer)
	{
		ExitStateFastDive();
		return;
	}
	TargetLayerZCenter = DiveLevelsActor->GetDiveBoundZCoord(TargetLayer, EDiveLayerBoundsID::Middle);
	Character->GetCharacterMovement()->MaxFlySpeed = FMath::Abs(TargetLayerZCenter - ZPosition)/CrossingLayerTime;
	DirectionScaleValue = ZPosition < TargetLayerZCenter ? -1 : 1;
}

void UFreeFallCharacterStateFastDive::ExitStateFastDive()
{
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
	else
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Idle);
	}
}
