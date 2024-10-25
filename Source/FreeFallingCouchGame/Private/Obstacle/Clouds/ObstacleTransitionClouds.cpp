﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Clouds/ObstacleTransitionClouds.h"

#include "Characters/FreeFallCharacter.h"
#include "GameMode/FreeFallGameMode.h"
#include "Math/UnitConversion.h"
#include "Settings/MapSettings.h"


// Sets default values
AObstacleTransitionClouds::AObstacleTransitionClouds()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObstacleTransitionClouds::BeginPlay()
{
	Super::BeginPlay();

	if(DistanceTolerance >= FVector::Dist(-SpawnExtent, SpawnExtent)/2)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "!!!! DISTANCE TOLERANCE IS TOO HIGH ! UNREAL WILL CRASH IF TRY RANDOMIZEPLAYERSPOSITION !!!!");
	}
	
}

void AObstacleTransitionClouds::StartTransition()
{
	//Wait for end of animation before randomizing player
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AObstacleTransitionClouds::RandomizePlayersPositions, DelayAfterStartTransition, false);
}

void AObstacleTransitionClouds::EndTransition()
{
	//Play end transition (clouds moving away)
}

void AObstacleTransitionClouds::RandomizePlayersPositions()
{
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString::Printf(TEXT("%f"), FVector::Dist(-SpawnExtent, SpawnExtent)));
	if(DistanceTolerance >= FVector::Dist(-SpawnExtent, SpawnExtent)/2)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "!!!! DISTANCE TOLERANCE IS TOO HIGH !!!!");
		return;
	}
	
	//Get existing characters
	const AFreeFallGameMode* FreeFallGameMode = Cast<AFreeFallGameMode>(GetWorld()->GetAuthGameMode());
	if(FreeFallGameMode == nullptr) return;
	if(FreeFallGameMode->CharactersInsideArena.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No Existing Character in Arena");
	
	TArray<AFreeFallCharacter*> CharactersInArena;
	CharactersInArena.Empty();
	CharactersInArena.Append(FreeFallGameMode->CharactersInsideArena);
	
	TArray<FVector> ExistingLocations;
	FVector NewLocation;

	for(AFreeFallCharacter* Player : CharactersInArena)
	{
		do
		{
			//Choose a random position
			NewLocation = FVector(
					FMath::RandRange(-SpawnExtent.X, SpawnExtent.X),
					FMath::RandRange(-SpawnExtent.Y, SpawnExtent.Y),
					FMath::RandRange(-SpawnExtent.Z, SpawnExtent.Z));
			NewLocation = NewLocation + GetActorLocation();
			IsNearAnyPlayer(NewLocation, DistanceTolerance, ExistingLocations);
		
		} while(IsNearAnyPlayer(NewLocation, DistanceTolerance, ExistingLocations)); //Check if valid

		ExistingLocations.Add(NewLocation);
		Player->SetActorLocation(NewLocation);
	}

	//Wait a bit for randomizing effect
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AObstacleTransitionClouds::EndTransition, DelayBeforeEndTransition, false);
}

bool AObstacleTransitionClouds::IsNearAnyPlayer(const FVector& SpawnPosition, float Tolerance, TArray<FVector>& SpawnedPositionList)
{
	if(SpawnedPositionList.Num() <= 0) return false;
	for (FVector SpawnedPosition : SpawnedPositionList)
	{
		float Distance = FVector::Dist(SpawnPosition, SpawnedPosition);
		if (Distance < Tolerance)
		{
			return true;
		}
	}

	return false;
}