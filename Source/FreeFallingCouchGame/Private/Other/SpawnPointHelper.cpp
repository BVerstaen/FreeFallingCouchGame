// Fill out your copyright notice in the Description page of Project Settings.


#include "Other/SpawnPointHelper.h"

#include "Other/DiveLayersID.h"
#include "Other/DiveLevels.h"


// Sets default values
ASpawnPointHelper::ASpawnPointHelper()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASpawnPointHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASpawnPointHelper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASpawnPointHelper::CenterOnCurrentDiveLevel()
{
	if (DiveLevelsActor!=nullptr)
	{
		FVector ActorLocation = GetActorLocation();
		float DiveBoundZCoord = DiveLevelsActor->GetDiveBoundZCoord(DiveLevelsActor->GetDiveLayersFromCoord(ActorLocation.Z), EDiveLayerBoundsID::Middle);
		SetActorLocation(FVector(ActorLocation.X, ActorLocation.Y, DiveBoundZCoord));
	}
}

