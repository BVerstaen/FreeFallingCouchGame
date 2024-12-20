// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateGrab.h"

#include "Audio/SoundSubsystem.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interface//GrabbableInterface.h"
#include "Other/Parachute.h"


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
	
	//Stop grabbing if release key
	if(!Character->bInputGrabPressed)
	{
		switch (Character->GrabbingState)
		{
			case EFreeFallCharacterGrabbingState::GrabPlayer:
				ReleasePlayerGrab(PreviousStateID);

				//Play end grab animation
				Character->PlayAnimation(EndGrabAnimation, false, true);
				break;
			
			case EFreeFallCharacterGrabbingState::GrabObject:
			case EFreeFallCharacterGrabbingState::GrabHeavierObject:
				ReleaseObjectGrab(PreviousStateID);

				//Play end grab animation
				Character->PlayAnimation(EndGrabAnimation, false, true);
				break;
			
			case EFreeFallCharacterGrabbingState::None:
			default: 
				break;
		}

		ExitStateConditions();
		return;
	}
	
	//Can't grab if is in Sensible Area
	if(Character->IsLookingToCloseToGrabber(ChainGrabAngleLimit))
		ExitStateConditions();
	
	//Play start grab animation
	Character->PlayAnimation(StartGrabAnimation, false, true);
	
	//Check if caught character
	if(Character->GrabbingState == EFreeFallCharacterGrabbingState::None)
	{
		if(PlayerGrab())
		{
			//play normal successful grab effect

			//Play grab sound
			USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
			SoundSubsystem->PlaySound("SFX_PLR_Grab_ST", Character, true);
		}
	}
	if(Character->GrabbingState == EFreeFallCharacterGrabbingState::None)
	{
		if(ObjectGrab())
		{
			//Play grab sound
			USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
			SoundSubsystem->PlaySound("SFX_PLR_Grab_ST", Character, true);
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
														EDrawDebugTrace::None,
														HitResult,
														true);

	
	//Has grabbed anything
	if (CanGrab)
	{
		//Has interface
		if(HitResult.GetActor()->Implements<UGrabbableInterface>())
		{
			return HitResult.GetActor();
		}

	}
	return nullptr;
}

void UFreeFallCharacterStateGrab::ReleasePlayerGrab(EFreeFallCharacterStateID PreviousStateID)
{
	PreviousState = PreviousStateID;
	Character->GrabbingState = EFreeFallCharacterGrabbingState::None;
	
	if(Character->OtherCharacterGrabbing)
	{
		//Reset grab rotation offset
		if(Character->OtherCharacterGrabbedBy)
		{
			//Calculate rotation offset
			Character->OtherCharacterGrabbedBy->GrabDefaultRotationOffset = Character->GetActorRotation() - Character->OtherCharacterGrabbedBy->GetActorRotation();
			Character->GrabDefaultRotationOffset = Character->OtherCharacterGrabbedBy->GetActorRotation() - Character->GetActorRotation();
		}
		
		//If release while diving -> then don't launch and set other to dive instead
		if(PreviousStateID == EFreeFallCharacterStateID::Dive)
		{
			Character->OtherCharacterGrabbing->GetStateMachine()->ChangeState(EFreeFallCharacterStateID::Dive);
			//Remove references
			Character->OtherCharacterGrabbing->StopEffectDeGrab();
			Character->OtherCharacterGrabbing->OtherCharacterGrabbedBy = nullptr;
			Character->OtherCharacterGrabbing = nullptr;
			
			//Exit Grab state
			ExitStateConditions();
			return;
		}
			
		//Launch other character
		FVector LaunchVelocity = Character->GetActorForwardVector() * LaunchOtherCharacterBaseLaunchMultiplier;
		LaunchVelocity += Character->GetMovementComponent()->Velocity * LaunchOtherCharacterForceMultiplier;
		Character->OtherCharacterGrabbing->LaunchCharacter(LaunchVelocity, true, true);

		//Play push sound
		USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
		SoundSubsystem->PlaySound("SFX_PLR_Collision_ST", Character, true);
		
		//Remove references
		Character->OtherCharacterGrabbing->StopEffectDeGrab();
		Character->OtherCharacterGrabbing->OtherCharacterGrabbedBy = nullptr;
		Character->OtherCharacterGrabbing = nullptr;
		
	}
}

void UFreeFallCharacterStateGrab::ReleaseObjectGrab(EFreeFallCharacterStateID PreviousStateID)
{

	if(Character->OtherObject)
	{
		FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
		if(Character->GrabbingState == EFreeFallCharacterGrabbingState::GrabObject)
		{
			//Launch other object
			FVector LaunchVelocity = Character->GetMovementComponent()->Velocity * LaunchObjectForceMultiplier;
			LaunchVelocity += Character->GetActorForwardVector() * LaunchObjectBaseLaunchMultiplier;
			//Get Actor's mesh
			if(UStaticMeshComponent* Mesh = Cast<UStaticMeshComponent>(Character->OtherObject->GetComponentByClass(UStaticMeshComponent::StaticClass())))
			{
				Mesh->SetSimulatePhysics(true);
				Mesh->AddImpulse(LaunchVelocity,NAME_None,false);
			}
		}
		else if(Character->GrabbingState == EFreeFallCharacterGrabbingState::GrabHeavierObject)
		{
			//Detach player from object
			Character->DetachFromActor(DetachmentRules);
		}
		
		//Null reference
		Character->GrabbingState = EFreeFallCharacterGrabbingState::None;
		Character->OtherObject = nullptr;
	}
	
	Character->GrabbingState = EFreeFallCharacterGrabbingState::None;
}

bool UFreeFallCharacterStateGrab::PlayerGrab() const
{
	//Find player to grab
 	AFreeFallCharacter* FoundCharacter = FindPlayerToGrab();
	if(!FoundCharacter) return false;
	
	//If found player has slurp
	if (!FoundCharacter->bIsGrabbable)
	{
		USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
		SoundSubsystem->PlaySound("SFX_PLR_SlurpGrab_ST", Character, true);
		
		return false;
	}
	
	//Can't grab the one I'm grabbing
	if(FoundCharacter == Character->OtherCharacterGrabbedBy) return false;

	//Steal Parachute if has one
	if(FoundCharacter->Parachute)
	{
		FoundCharacter->Parachute->StealParachute(FoundCharacter, Character);
	}

	//Grab onto the player
	Character->GrabbingState = EFreeFallCharacterGrabbingState::GrabPlayer;

	//Check if can steal grab
	if(FoundCharacter->OtherCharacterGrabbedBy)
	{
		//Remove References
		FoundCharacter->OtherCharacterGrabbedBy->OtherCharacterGrabbing->GrabbingState = EFreeFallCharacterGrabbingState::None;
		FoundCharacter->OtherCharacterGrabbedBy->OtherCharacterGrabbing = nullptr;
		FoundCharacter->OtherCharacterGrabbedBy = nullptr;
	}
	
	//Set cross-references
	Character->OtherCharacterGrabbing = FoundCharacter;
	Character->OtherCharacterGrabbing->OtherCharacterGrabbedBy = Character;
	
	//Calculate location offset
	FVector GrabOffset = FoundCharacter->GetActorLocation() - Character->GetActorLocation();
	if(GrabOffset.Size() <= GrabMinimumDistance || GrabOffset.Size() >= GrabMaximumDistance)
	{
		FoundCharacter->SetActorLocation(Character->GetActorLocation() + Character->GetActorForwardVector() * GrabMinimumDistance);
		GrabOffset = FoundCharacter->GetActorLocation() - Character->GetActorLocation();
	}
	Character->GrabInitialOffset = Character->GetActorRotation().UnrotateVector(GrabOffset);

	//Calculate rotation offset
	Character->GrabDefaultRotationOffset = FoundCharacter->GetActorRotation() - Character->GetActorRotation();
	FoundCharacter->GrabDefaultRotationOffset = Character->GetActorRotation() - FoundCharacter->GetActorRotation();

	//Set Degrab counter
	FoundCharacter->ActivateDeGrab();
	
	return true;
}

bool UFreeFallCharacterStateGrab::ObjectGrab() const
{
	//Find actor to grab
	AActor* FoundActor = FindActorToGrab();
	if(!FoundActor) return false;

	//If so, check if has grabbable interface
	IGrabbableInterface* IGrabbableActor = Cast<IGrabbableInterface>(FoundActor);
	if(!IGrabbableActor) return false;

	//If so, check if can be taken
	if(IGrabbableActor->CanBeTaken())
	{
		IGrabbableActor->Use(Character);
		return false;
	}
	
	//Else check if can be grabbed
	if(!IGrabbableActor->CanBeGrabbed()) return false;
	Character->OtherObject = FoundActor;
		
	//If is an obstacle
	if(AObstacle* FoundObstacle = Cast<AObstacle>(FoundActor))
	{
		//Check who's heavier
		if(FoundObstacle->GetMass() > Character->GetMass())
		{
			Character->GrabbingState = EFreeFallCharacterGrabbingState::GrabHeavierObject;
			Character->GetMovementComponent()->Velocity = FVector(0, 0, 0);
			Character->GrabHeavyObjectRelativeLocationPoint = Character->GetActorLocation() - FoundObstacle->GetActorLocation();
		}
		else
		{
			Character->GrabbingState = EFreeFallCharacterGrabbingState::GrabObject;
				
			FoundObstacle->Mesh->ComponentVelocity = FVector(0, 0, 0);
			FoundObstacle->Mesh->SetSimulatePhysics(true);

			FAttachmentTransformRules AttachementRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
			FoundActor->AttachToComponent(Character->GetObjectGrabPoint(), AttachementRules);
			//Detach object
			FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
			Character->OtherObject->DetachFromActor(DetachmentRules);

			Character->GrabDefaultRotationOffset = Character->GetActorRotation() - FoundObstacle->GetActorRotation();
		}
	}
	
	return true;
}
