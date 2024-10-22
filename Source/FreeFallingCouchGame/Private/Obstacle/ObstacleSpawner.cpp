// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleSpawner.h"

AObstacleSpawner::AObstacleSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObstacleMinSpeed = 0;
	ObstacleMaxSpeed = 1;
	ObstacleMinTimer = 0;
	ObstacleMaxTimer = 1;
}

void AObstacleSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	//Set spawner timer
	float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AObstacleSpawner::SpawnObstacle, SpawnDelay, false, SpawnDelay);
}

void AObstacleSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugBox(GetWorld(), GetTransform().GetLocation(), ObstacleSpawnExtant, FColor::Green, false, .1, 0,1);
}

FVector AObstacleSpawner::GetRandomLocationSpawnVector() const
{
	FVector LocalRandomPosition = FVector(
		FMath::RandRange(-ObstacleSpawnExtant.X, ObstacleSpawnExtant.X),
		FMath::RandRange(-ObstacleSpawnExtant.Y, ObstacleSpawnExtant.Y),
		FMath::RandRange(-ObstacleSpawnExtant.Z, ObstacleSpawnExtant.Z)
		);
	return GetTransform().GetLocation() + LocalRandomPosition; 
}

void AObstacleSpawner::SpawnObstacle()
{
	//Check if enough Obstacle
	if(ObstaclesList.Num() <= 0) return;
	
	//Get random obstacle
	TSubclassOf<AObstacle> ObstacleToSpawn = ObstaclesList[FMath::RandRange(0, ObstaclesList.Num() - 1)];

	//Setup spawning transform (with random position)
	FTransform SpawnTransform = GetTransform();
	FVector SpawnLocation = GetRandomLocationSpawnVector();
	SpawnTransform.SetLocation(SpawnLocation);
	
	//SpawnTransform.SetLocation()
	AObstacle* SpawningObstacle = GetWorld()->SpawnActorDeferred<AObstacle>(
		ObstacleToSpawn,
		SpawnTransform
		);

	//Setup obstacle
	float ObstacleSpeed = FMath::RandRange(ObstacleMinSpeed, ObstacleMaxSpeed);
	SpawningObstacle->Speed = ObstacleSpeed;
	SpawningObstacle->Direction = ObstacleDirection;
	SpawningObstacle->FinishSpawning(SpawnTransform);

	//Reset timer with new delay
	SpawnTimer.Invalidate();
	float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AObstacleSpawner::SpawnObstacle, SpawnDelay, false, SpawnDelay);

}

void AObstacleSpawner::PauseSpawner()
{
	//Reset timer with new delay
	SpawnTimer.Invalidate();
}

void AObstacleSpawner::ResumeSpawner()
{
	//Reset timer with new delay
    SpawnTimer.Invalidate();
    float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
    GetWorldTimerManager().SetTimer(SpawnTimer, this, &AObstacleSpawner::SpawnObstacle, SpawnDelay, false, SpawnDelay);
}
