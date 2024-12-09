// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleSpawner.h"

#include "Audio/SoundSubsystem.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"

AObstacleSpawner::AObstacleSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ObstacleMinSpeed = 0;
	ObstacleMaxSpeed = 1;
	ObstacleMinTimer = 0;
	ObstacleMaxTimer = 1;

	bPlaySpawnTimer = true;
}

void AObstacleSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	//Set spawner timer
	if(bPlaySpawnTimer)
	{
		//Start Timer after round start
		if(AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GameMode->OnStartRound.AddDynamic(this, &AObstacleSpawner::StartTimer);
		}
	}
}

void AObstacleSpawner::StartTimer()
{
	if(bPlaySpawnTimer)
	{
		float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &AObstacleSpawner::SpawnObstacleEvent, SpawnDelay, false, SpawnDelay);
	}
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


AObstacle* AObstacleSpawner::SpawnObstacle()
{
	//Check if enough Obstacle
	if(ObstaclesList.Num() <= 0) return nullptr;
	
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
	//Rotate Obstacle
	FVector ObstacleFinalTorqueRotation = FVector(
		FMath::RandRange(ObstacleMinTorqueRotation.X, ObstacleMaxTorqueRotation.X) * 100,
		FMath::RandRange(ObstacleMinTorqueRotation.Y, ObstacleMaxTorqueRotation.Y) * 100,
		FMath::RandRange(ObstacleMinTorqueRotation.Z, ObstacleMaxTorqueRotation.Z) * 100
		);
	if(SpawningObstacle->Mesh)
		SpawningObstacle->Mesh->AddTorqueInDegrees(ObstacleFinalTorqueRotation, NAME_None, true);

	if(bFaceSpawner)
	{
		FVector FaceDirection = -GetActorLocation().GetSafeNormal();
		FaceDirection.Z = 0;
		SpawningObstacle->SetActorRotation(FaceDirection.Rotation());
	}
	
	SpawningObstacle->FinishSpawning(SpawnTransform);

	//Play obstacle spawns sounds
	for(FName SoundName : SpawningObstacle->SoundsOnSpawn)
	{
		if(SoundName == NAME_None) continue;

		USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
		SoundSubsystem->PlaySound(SoundName, SpawningObstacle, false);
	}
	
	GetWorldTimerManager().ClearTimer(SpawnTimer);
	//Reset timer
	if(bPlaySpawnTimer)
	{
		float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
		GetWorldTimerManager().SetTimer(SpawnTimer, this, &AObstacleSpawner::SpawnObstacleEvent, SpawnDelay, false);		
	}

	return SpawningObstacle;
}

void AObstacleSpawner::SpawnObstacleEvent()
{
	GEngine->AddOnScreenDebugMessage(-1,10.0f, FColor::Blue, "Spawn");
	SpawnObstacle();
}

void AObstacleSpawner::PauseSpawner()
{
	bPlaySpawnTimer = false;
	
	//Reset timer with new delay
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}

void AObstacleSpawner::RestartSpawner()
{
	bPlaySpawnTimer = true;
	
	//Reset timer with new delay
	GetWorldTimerManager().ClearTimer(SpawnTimer);
    float SpawnDelay = FMath::RandRange(ObstacleMinTimer, ObstacleMaxTimer);
    GetWorldTimerManager().SetTimer(SpawnTimer, this, &AObstacleSpawner::SpawnObstacleEvent, SpawnDelay, false);
}
