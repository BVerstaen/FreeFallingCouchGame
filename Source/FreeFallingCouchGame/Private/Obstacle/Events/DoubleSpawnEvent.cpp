// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Events/DoubleSpawnEvent.h"


// Sets default values
ADoubleSpawnEvent::ADoubleSpawnEvent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ADoubleSpawnEvent::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoubleSpawnEvent::TriggerEvent()
{
	Super::TriggerEvent();

	for(AObstacleSpawnerManager* SpawnerManager : SpawnerManagers)
	{
		SpawnerManager->bDoubleSpawn = true;
	}

	OnEventEnded.Broadcast(this);
}

