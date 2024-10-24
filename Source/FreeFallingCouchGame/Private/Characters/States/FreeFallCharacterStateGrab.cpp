// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/States/FreeFallCharacterStateGrab.h"

#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "Kismet/KismetSystemLibrary.h"

EFreeFallCharacterStateID UFreeFallCharacterStateGrab::GetStateID()
{
	return EFreeFallCharacterStateID::Grab;
}

void UFreeFallCharacterStateGrab::StateEnter(EFreeFallCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	GEngine->AddOnScreenDebugMessage(
	 	-1,
	 	3.f,
	 	FColor::Green,
	 	TEXT("Enter State Grab")
	 	);
	
	AFreeFallCharacter* FoundCharacter = FindPlayerToGrab();
	if(!FoundCharacter)
	{
		StateMachine->ChangeState(EFreeFallCharacterStateID::Idle);
	}
}

void UFreeFallCharacterStateGrab::StateExit(EFreeFallCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
	 -1,
	 3.f,
	 FColor::Green,
	 TEXT("Exit State Grab")
	 );
}

void UFreeFallCharacterStateGrab::StateTick(float DeltaTime)
{
	Super::StateTick(DeltaTime);
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
