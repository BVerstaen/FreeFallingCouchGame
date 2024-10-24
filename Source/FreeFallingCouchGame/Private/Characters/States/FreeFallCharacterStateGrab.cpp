// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateGrab.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"


EFreeFallCharacterStateID UFreeFallCharacterStateGrab::GetStateID()
{
	return EFreeFallCharacterStateID::Grab;
}

void UFreeFallCharacterStateGrab::StateInit(UFreeFallCharacterStateMachine* InStateMachine)
{
	Super::StateInit(InStateMachine);
	//Pass fields to Character
	Character->GrabRotationSpeed = RotationSpeed;
	Character->GrabRotationInfluenceStrength = RotationInfluenceStrength;
}

void UFreeFallCharacterStateGrab::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	//Stop grabbing if release key
	if(!Character->bIsGrabbing)
	{
		if(Character->OtherCharacter)
		{
			//Launch other character
			FVector LaunchVelocity = Character->GetMovementComponent()->Velocity * LaunchOtherCharacterForceMultiplier;
			LaunchVelocity += Character->GetActorForwardVector() * LaunchOtherCharacterBaseLaunchMultiplier;
			Character->OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);

			//Remove reference
			Character->OtherCharacter->OtherCharacter = nullptr;
			Character->OtherCharacter = nullptr;
			
		}

		//Exit Grab state
		ExitStateConditions();
		return;
	}

	
	//Check if caught character
	AFreeFallCharacter* FoundCharacter = FindPlayerToGrab();
	if(FoundCharacter)	//Change state if couldn't find a player
	{
		//Check if not trying to grab the character that is grabbing you
		if(FoundCharacter != Character->OtherCharacter)
		{
			//Set cross-references
			Character->OtherCharacter = FoundCharacter;
			Character->OtherCharacter->OtherCharacter = Character;
		
			//Calculate location offset
			FVector GrabOffset = FoundCharacter->GetActorLocation() - Character->GetActorLocation();
			Character->GrabInitialOffset = Character->GetActorRotation().UnrotateVector(GrabOffset);

			//Calculate rotation offset
			Character->GrabDefaultRotationOffset = FoundCharacter->GetActorRotation().Yaw - Character->GetActorRotation().Yaw;
			FoundCharacter->GrabDefaultRotationOffset = Character->GetActorRotation().Yaw - FoundCharacter->GetActorRotation().Yaw;
		}
	}
	ExitStateConditions();
}

void UFreeFallCharacterStateGrab::ExitStateConditions() const
{
	if(Character->GetInputMove() != FVector2D::ZeroVector)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Move);
		return;
	}
	StateMachine->ChangeState(EFreeFallCharacterStateID::Idle);
}

AFreeFallCharacter* UFreeFallCharacterStateGrab::FindPlayerToGrab() const
{
	FTransform CharacterTransform = Character->GetTransform();
	
	FVector SphereLocation = CharacterTransform.GetLocation() + CharacterTransform.GetRotation().GetForwardVector() * GrabForwardDistance;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	TArray<AActor*> ignoreActors;
	FHitResult HitResult;
	
	bool CanGrab = UKismetSystemLibrary::SphereTraceSingleForObjects(
														GetWorld(),
														SphereLocation,
														SphereLocation,
														GrabRadius,
														traceObjectTypes,
														false,
														ignoreActors,
														EDrawDebugTrace::ForOneFrame,
														HitResult,
														true);
	if (CanGrab)
	{
		if(AFreeFallCharacter* OtherActor =  Cast<AFreeFallCharacter>(HitResult.GetActor()))
		{
			return OtherActor;
		}
	}
	return nullptr;
}
