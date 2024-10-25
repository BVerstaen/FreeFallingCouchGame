﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateGrab.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interface//GrabbableInterface.h"
#include "Kismet/KismetStringLibrary.h"


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
	//Get previous state ID

	//Can't grab if is grabbed
	if(Character->OtherCharacter && !(Character->GrabbingState == EFreeFallCharacterGrabbingState::GrabPlayer))
		ExitStateConditions();
	
	//Stop grabbing if release key
	if(!Character->bInputGrabPressed)
	{
		switch (Character->GrabbingState)
		{
			case EFreeFallCharacterGrabbingState::GrabPlayer:
				ReleasePlayerGrab(PreviousStateID);
				break;
			
			case EFreeFallCharacterGrabbingState::GrabObject:
				ReleaseObjectGrab(PreviousStateID);
				break;
			
			case EFreeFallCharacterGrabbingState::None:
			default: 
				break;
		}

		ExitStateConditions();
		return;
	}

	
	//Check if caught character
	if(Character->GrabbingState == EFreeFallCharacterGrabbingState::None)
		PlayerGrab();
	if(Character->GrabbingState == EFreeFallCharacterGrabbingState::None)
		ObjectGrab();
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

AActor* UFreeFallCharacterStateGrab::FindActorToGrab() const
{
	FTransform CharacterTransform = Character->GetTransform();
	
	FVector SphereLocation = CharacterTransform.GetLocation() + CharacterTransform.GetRotation().GetForwardVector() * GrabForwardDistance;
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_WorldDynamic);
	TArray<AActor*> ignoreActors;
	FHitResult HitResult;
	
	bool CanGrab = UKismetSystemLibrary::SphereTraceSingle(
														GetWorld(),
														SphereLocation,
														SphereLocation,
														GrabRadius,
														TraceChannel,
														false,
														ignoreActors,
														EDrawDebugTrace::ForOneFrame,
														HitResult,
														true);

	
	//Has grabbed anything
	if (CanGrab)
	{
		//Has interface
		if(HitResult.GetActor()->Implements<UGrabbableInterface>())
		{
			//If so, check if can be grabbed
			IGrabbableInterface* IGrabbableActor = Cast<IGrabbableInterface>(HitResult.GetActor());
			if(IGrabbableActor->CanBeGrabbed())
				return HitResult.GetActor();
		}

	}
	return nullptr;
}

void UFreeFallCharacterStateGrab::ReleasePlayerGrab(EFreeFallCharacterStateID PreviousStateID)
{
	PreviousState = PreviousStateID;
	Character->GrabbingState = EFreeFallCharacterGrabbingState::None;
	
	if(Character->OtherCharacter)
	{
		//If release while diving -> then don't launch and set other to dive instead
		if(PreviousStateID == EFreeFallCharacterStateID::Dive)
		{
			Character->OtherCharacter->GetStateMachine()->ChangeState(EFreeFallCharacterStateID::Dive);
			//Exit Grab state
			ExitStateConditions();
			return;
		}
			
		//Launch other character
		FVector LaunchVelocity = Character->GetMovementComponent()->Velocity * LaunchOtherCharacterForceMultiplier;
		LaunchVelocity += Character->GetActorForwardVector() * LaunchOtherCharacterBaseLaunchMultiplier;
		Character->OtherCharacter->LaunchCharacter(LaunchVelocity, true, true);
			
		//Remove reference
		Character->OtherCharacter->OtherCharacter = nullptr;
		Character->OtherCharacter = nullptr;
			
	}
}

void UFreeFallCharacterStateGrab::ReleaseObjectGrab(EFreeFallCharacterStateID PreviousStateID)
{
	Character->GrabbingState = EFreeFallCharacterGrabbingState::None;

	if(Character->OtherObject || Character->GrabbingState == EFreeFallCharacterGrabbingState::GrabObject)
	{
		//Detach object
		FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		Character->OtherObject->DetachFromActor(DetachmentRules);
		
		//Launch other object
		FVector LaunchVelocity = Character->GetMovementComponent()->Velocity * LaunchObjectForceMultiplier;
		LaunchVelocity += Character->GetActorForwardVector() * LaunchObjectBaseLaunchMultiplier;
		//Get Actor's mesh
		if(UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Character->OtherObject->GetComponentByClass(UStaticMeshComponent::StaticClass())))
		{
			Mesh->SetSimulatePhysics(true);
			Mesh->AddImpulse(LaunchVelocity,NAME_None,false);
		}
		
		//Null reference
		Character->OtherObject = nullptr;
	}
}

void UFreeFallCharacterStateGrab::PlayerGrab() const
{
 	AFreeFallCharacter* FoundCharacter = FindPlayerToGrab();
	if(FoundCharacter)	//Change state if couldn't find a player
	{
		Character->GrabbingState = EFreeFallCharacterGrabbingState::GrabPlayer;
		
		//Set cross-references
		Character->OtherCharacter = FoundCharacter;
		Character->OtherCharacter->OtherCharacter = Character;
		//Calculate location offset
		FVector GrabOffset = FoundCharacter->GetActorLocation() - Character->GetActorLocation();
		if(GrabOffset.Size() <= GrabMinimumDistance)
		{
			FoundCharacter->SetActorLocation(Character->GetActorLocation() + Character->GetActorForwardVector() * GrabMinimumDistance);
			GrabOffset = FoundCharacter->GetActorLocation() - Character->GetActorLocation();
		}
		Character->GrabInitialOffset = Character->GetActorRotation().UnrotateVector(GrabOffset);

		//Calculate rotation offset
		Character->GrabDefaultRotationOffset = FoundCharacter->GetActorRotation() - Character->GetActorRotation();
		FoundCharacter->GrabDefaultRotationOffset = Character->GetActorRotation() - FoundCharacter->GetActorRotation();
	}
}

void UFreeFallCharacterStateGrab::ObjectGrab() const
{
	AActor* FoundActor = FindActorToGrab();
	if(FoundActor)
	{
		Character->OtherObject = FoundActor;
		
		//If is an obstacle
		if(const AObstacle* FoundObstacle = Cast<AObstacle>(FoundActor))
		{
			//Check who's heavier
			if(FoundObstacle->Mesh->GetMass() > Character->GetPlayerMass())
			{
				Character->GrabbingState = EFreeFallCharacterGrabbingState::GrabHeavierObject;
			}
			else
			{
				Character->GrabbingState = EFreeFallCharacterGrabbingState::GrabObject;
				
				FoundObstacle->Mesh->ComponentVelocity = FVector(0, 0, 0);
				FoundObstacle->Mesh->SetSimulatePhysics(false);
				FAttachmentTransformRules AttachementRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
				FoundActor->AttachToComponent(Character->GetObjectGrabPoint(), AttachementRules);
			}
		}
		
	}
}
