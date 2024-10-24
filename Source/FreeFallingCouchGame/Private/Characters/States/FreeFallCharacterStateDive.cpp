// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateDive.h"

#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

EFreeFallCharacterStateID UFreeFallCharacterStateDive::GetStateID()
{
	return EFreeFallCharacterStateID::Dive;
}

void UFreeFallCharacterStateDive::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	Character->GetCharacterMovement()->MaxFlySpeed = StartDiveSpeed;
	AccelerationAlpha = 0.f;
	
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

	float InputDive = Character->GetInputDive();

	GEngine->AddOnScreenDebugMessage(
		-1,
		DeltaTime,
		FColor::Red,
		TEXT("InputDive = " + FString::SanitizeFloat(InputDive) + " and ActorLocation : " + FString::SanitizeFloat(Character->GetActorLocation().Z) + " and DefaultZLocation : " + FString::SanitizeFloat(Character->GetDefaultZPosition())));

	AccelerationAlpha += DeltaTime;
	Character->GetCharacterMovement()->MaxFlySpeed = FMath::Lerp(StartDiveSpeed,MaxDiveSpeed,FMath::Min(AccelerationAlpha/ReachMaxSpeedTime,1));

	if (FMath::Abs(InputDive) < CharactersSettings->InputMoveThreshold)
	{
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
	else
	{
		if (Character->GetDefaultZPosition() + CharactersSettings->DiveDistance > Character->GetActorLocation().Z && InputDive < 0)
		{
			FVector Direction = Character->GetCameraActor()->GetActorForwardVector();
			Direction.Normalize();
			Character->AddMovementInput(Direction, InputDive);
		}
		else if (InputDive < 0)GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Can't go up"));
		
		if (Character->GetDefaultZPosition() - CharactersSettings->DiveDistance < Character->GetActorLocation().Z && InputDive > 0)
		{
			FVector Direction = Character->GetCameraActor()->GetActorForwardVector();
			Direction.Normalize();
			Character->AddMovementInput(Direction, InputDive);
		}
		else if (InputDive > 0) GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,TEXT("Can't go down"));
		Character->SetDiveMaterialColor();
	}
}
