// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateFastDive.h"

#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Haptic/HapticsStatics.h"
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
	Character->EnterDiveEvent();
	OldFlySpeed = Character->GetCharacterMovement()->MaxFlySpeed;

	if(Character->GetLockControls())
	{
		Character->GetStateMachine()->ChangeState(EFreeFallCharacterStateID::Idle);
		return;
	}
	

	Character->bIsDiveForced = false;
	OldInputDive = 0.0f;
	
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
	PlayHaptics();
	
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		"Entering StateFastDive");	
}

void UFreeFallCharacterStateFastDive::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	Character->ExitDiveEvent();

	Character->bIsDiveForced = true;

	Character->GetCharacterMovement()->MaxFlySpeed = OldFlySpeed;
}

void UFreeFallCharacterStateFastDive::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);

	if (Character->GetCharacterMovement()->MovementMode !=MOVE_Flying)
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, "you ain't flying darling");
	//GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Cyan, "StateFastDive Tick");
	GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Purple, "FlySpeed = " + FString::SanitizeFloat(Character->GetCharacterMovement()->MaxFlySpeed));
	
	FVector Direction = Character->GetActorLocation() - Character->GetCameraActor()->GetActorLocation();
	Direction.Normalize();

	//GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Purple, "CharZLoc : " + FString::SanitizeFloat(Character->GetActorLocation().Z));
	//GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Purple, "ZTarget : " + FString::SanitizeFloat(TargetLayerZCenter));

	//Change animation based on dive direction
	if(OldInputDive != DirectionScaleValue)
	{
		OldInputDive = DirectionScaleValue;
		if(DirectionScaleValue > 0)
			Character->PlayAnimation(DiveDownwardsAnimation, true);
		else
			Character->PlayAnimation(DiveUpwardsAnimation, true);
	}
	

	if (Character->GetActorLocation().Z > TargetLayerZCenter && DirectionScaleValue < 0)
	{
		FVector Velocity = Character->GetCharacterMovement()->Velocity;
		Character->GetCharacterMovement()->Velocity += Velocity.Z * Direction;
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple, "Velocity with DSV-1 : " + FString::SanitizeFloat(Velocity.Z));
		ExitStateFastDive();
		return;
	}
	if (Character->GetActorLocation().Z < TargetLayerZCenter && DirectionScaleValue > 0)
	{
		FVector Velocity = Character->GetCharacterMovement()->Velocity;
		Character->GetCharacterMovement()->Velocity += Velocity.Z * Direction;
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple, "Velocity with DSV1 : " + FString::SanitizeFloat(Velocity.Z));
		ExitStateFastDive();
		return;
	}
	Character->AddMovementInput(Direction, DirectionScaleValue,true);
	//Character->GetCharacterMovement()->Velocity = Direction *  Character->GetCharacterMovement()->MaxFlySpeed * DirectionScaleValue;

	//Change Angle by rotation
	Character->InterpMeshPlayer(FRotator(Character->GetPlayerDefaultRotation().Pitch, Character->GetMesh()->GetRelativeRotation().Yaw, DirectionScaleValue * MeshMovementRotationAngle), DeltaTime, MeshMovementDampingSpeed);
}

void UFreeFallCharacterStateFastDive::CheckTargetedLayer()
{
	const float ZPosition = Character->GetActorLocation().Z;
	const EDiveLayersID CurrentLayer = DiveLevelsActor->GetDiveLayersFromCoord(ZPosition);
	if (bSmartDiveInput && DiveLevelsActor->bDisableTopLayer)
	{
		TargetLayer = CurrentLayer == EDiveLayersID::Middle? EDiveLayersID::Bottom : EDiveLayersID::Middle;
	}
	else if (Character->GetInputFastDive() < 0)
	{
		switch (CurrentLayer)
		{
		case EDiveLayersID::Top:
			TargetLayer = CurrentLayer;
			break;
		case EDiveLayersID::Middle:
			TargetLayer = DiveLevelsActor->bDisableTopLayer ? CurrentLayer : EDiveLayersID::Top;
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
		GEngine->AddOnScreenDebugMessage(-1,100.f, FColor::Cyan, "Jparsla");
		return;
	}
	TargetLayerZCenter = DiveLevelsActor->GetDiveBoundZCoord(TargetLayer, EDiveLayerBoundsID::Middle);
	Character->GetCharacterMovement()->MaxFlySpeed = FMath::Abs(TargetLayerZCenter - ZPosition)/FMath::Max(0,CrossingLayerTime);
	DirectionScaleValue = ZPosition < TargetLayerZCenter ? -1 : 1;
	/*GEngine->AddOnScreenDebugMessage(-1,100.f, FColor::Purple, "FlySpeed = " + FString::SanitizeFloat(Character->GetCharacterMovement()->MaxFlySpeed));
	GEngine->AddOnScreenDebugMessage(-1,100.f, FColor::Purple, "ZPosition = " + FString::SanitizeFloat(ZPosition));
	GEngine->AddOnScreenDebugMessage(-1,100.f, FColor::Purple, "TargetLayerZCenter = " + FString::SanitizeFloat(TargetLayerZCenter));
	GEngine->AddOnScreenDebugMessage(-1,100.f, FColor::Purple, "CrossingLayerTime = " + FString::SanitizeFloat(CrossingLayerTime));
	*/
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

void UFreeFallCharacterStateFastDive::PlayHaptics()
{
	APlayerController* PlayerController = Cast<APlayerController>(Character->Controller);
	const EDiveLayersID CurrentLayer = DiveLevelsActor->GetDiveLayersFromCoord(Character->GetActorLocation().Z);
	switch (CurrentLayer)
	{
	case EDiveLayersID::Bottom:
		UHapticsStatics::CallHapticsDive(this, PlayerController, false);
		break;
	case EDiveLayersID::Middle:
		UHapticsStatics::CallHapticsDive(this, PlayerController, TargetLayer==EDiveLayersID::Bottom);
		break;
	case EDiveLayersID::Top:
		UHapticsStatics::CallHapticsDive(this, PlayerController, true);
		break;
	default: 
		break;
	}
}
