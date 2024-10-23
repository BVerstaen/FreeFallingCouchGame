// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleSpawnerManager.h"


// Sets default values
AObstacleSpawnerManager::AObstacleSpawnerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObstacleSpawnerManager::BeginPlay()
{
	Super::BeginPlay();

	//Disable every spawner
	for(AObstacleSpawner* ObstacleSpawner : LinkedObstacleSpawners)
	{
		ObstacleSpawner->bPlaySpawnTimer = false;
		ObstacleSpawner->PauseSpawner();
	}

	
	//Start default timer
	float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
	GetWorldTimerManager().SetTimer(SpawnerTimer, this, &AObstacleSpawnerManager::LaunchSpawn, SpawnDelay, false);
}

void AObstacleSpawnerManager::LaunchSpawn()
{
	//Spawn Random timer
	AObstacleSpawner* Spawner = LinkedObstacleSpawners[FMath::RandHelper(LinkedObstacleSpawners.Num())];
	Spawner->SpawnObstacle();

	//Reset Timer
	GetWorldTimerManager().ClearTimer(SpawnerTimer);
	float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
	GetWorldTimerManager().SetTimer(SpawnerTimer, this, &AObstacleSpawnerManager::LaunchSpawn, SpawnDelay, false);
}


