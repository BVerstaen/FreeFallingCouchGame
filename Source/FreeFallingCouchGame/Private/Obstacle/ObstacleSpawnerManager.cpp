// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleSpawnerManager.h"

#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"


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

	//Start Timer after round start
	if(AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnStartRound.AddDynamic(this, &AObstacleSpawnerManager::StartTimer);
	}
}

void AObstacleSpawnerManager::StartTimer()
{
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

void AObstacleSpawnerManager::PauseTimer()
{
	bTimerPlaying = false;
	GetWorldTimerManager().PauseTimer(SpawnerTimer);
}

void AObstacleSpawnerManager::ResumeTimer()
{
	bTimerPlaying = true;
	GetWorldTimerManager().UnPauseTimer(SpawnerTimer);
}

bool AObstacleSpawnerManager::IsTimerPlaying()
{
	return bTimerPlaying;
}

/* Faudrait idéalement lancer la delegate au début pour que le bon spawner s'initialize
 * Doit également être bind à la delegate 
 */
void AObstacleSpawnerManager::ActivateCamera(int NewCamPosition)
{
	if(CameraPositionIndex.Contains(NewCamPosition))
	{
		ResumeTimer();
	}
	else
	{
		PauseTimer();
	}
}


