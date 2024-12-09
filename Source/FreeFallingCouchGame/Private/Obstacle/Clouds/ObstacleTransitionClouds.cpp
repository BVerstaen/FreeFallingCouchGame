// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Clouds/ObstacleTransitionClouds.h"

#include "Audio/SoundSubsystem.h"
#include "Characters/FreeFallCharacter.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnitConversion.h"
#include "Obstacle/Obstacle.h"
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
	OnTransitionStarted();
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AObstacleTransitionClouds::RandomizePlayersPositions, DelayAfterStartTransition, false);
}

void AObstacleTransitionClouds::EndTransition()
{
	//Play end transition (clouds moving away)
	OnEventEnded.Broadcast(this);
}

void AObstacleTransitionClouds::RandomizePlayersPositions()
{
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, FString::Printf(TEXT("%f"), FVector::Dist(-SpawnExtent, SpawnExtent)));
	if(DistanceTolerance >= FVector::Dist(-SpawnExtent, SpawnExtent)/2)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "!!!! DISTANCE TOLERANCE IS TOO HIGH !!!!");
		return;
	}

	//Play randomizer sound
	USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_GPE_weather_ST", this, true);
	
	//Get existing characters
	const AFreeFallGameMode* FreeFallGameMode = Cast<AFreeFallGameMode>(GetWorld()->GetAuthGameMode());
	if(FreeFallGameMode == nullptr) return;
	if(FreeFallGameMode->CharactersInsideArena.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No Existing Character in Arena");
	
	TArray<AFreeFallCharacter*> CharactersInArena;
	CharactersInArena.Empty();
	CharactersInArena.Append(FreeFallGameMode->CharactersInsideArena);
	
	TArray<FVector> ExistingLocations;
	FVector NewLocation;

	//Get every obstacle as ExistingLocations
	GetAllNearObstacles(ExistingLocations);
	
	//Teleport player randomly
	for(AFreeFallCharacter* Player : CharactersInArena)
	{
		if(!Player)
			continue;
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

		//Stop grabbing
		Player->GrabbingState = EFreeFallCharacterGrabbingState::None;
		Player->OtherCharacterGrabbedBy = nullptr;
		Player->OtherCharacterGrabbing = nullptr;
		Player->StopEffectDeGrab();

		//Play scream sound
		SoundSubsystem->PlaySound("VOC_PLR_scream_ST", this, true);
	}

	//Wait a bit for randomizing effect
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AObstacleTransitionClouds::EndTransition, DelayBeforeEndTransition, false);
	OnTransitionCompleted();
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

void AObstacleTransitionClouds::GetAllNearObstacles(TArray<FVector>& SpawnedPositionList)
{
	FVector SelfLocation = GetTransform().GetLocation();
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_WorldDynamic);
	TArray<AActor*> ignoreActors;
	TArray<FHitResult> HitResults;
	
	bool CanGrab = UKismetSystemLibrary::BoxTraceMulti(
														GetWorld(),
														SelfLocation,
														SelfLocation,
														SpawnExtent,
														GetTransform().GetRotation().Rotator(),
														TraceChannel,
														false,
														ignoreActors,
														EDrawDebugTrace::None,
														HitResults,
														true);

	
	//Has grabbed anything
	if (CanGrab)
	{
		for (FHitResult HitResult : HitResults)
		{
			if(Cast<AObstacle>(HitResult.GetActor()))
			{
				SpawnedPositionList.Add(HitResult.Location);
			}
		}
	}
}

void AObstacleTransitionClouds::TriggerEvent()
{
	Super::TriggerEvent();

	StartTransition();
}
