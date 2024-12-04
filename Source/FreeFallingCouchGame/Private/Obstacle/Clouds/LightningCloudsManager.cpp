// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Clouds/LightningCloudsManager.h"

#include "Audio/SoundSubsystem.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Obstacle/Clouds/LightningCloud.h"


// Sets default values
ALightningCloudsManager::ALightningCloudsManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALightningCloudsManager::BeginPlay()
{
	Super::BeginPlay(); //<- Spawn des Lightnings
}

// Called every frame
void ALightningCloudsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsSpawningClouds) return;
	
	SpawnClock += DeltaTime;
	if (SpawnClock >= SpawnCooldown)
	{
		SpawnedLightning++;
		SpawnCooldown = FMath::RandRange(MinSpawnCooldown, MaxSpawnCooldown);
		SpawnClock = 0.f;
		CreateLightningCloud();
		if (SpawnedLightning == NbLightning) bIsSpawningClouds = false;
	}
}

void ALightningCloudsManager::TriggerEvent()
{
	Super::TriggerEvent();

	GEngine->AddOnScreenDebugMessage(-1,10.f,FColor::Cyan,"LightningCloudsManager::TriggerEvent");
	bIsSpawningClouds = true;
	SpawnCooldown = FMath::RandRange(MinSpawnCooldown, MaxSpawnCooldown);
	SpawnClock = 0.f;
	SpawnedLightning = 0;
	FinishedLightning = 0;
}

void ALightningCloudsManager::LightningStrucked(ALightningCloud* LightningCloudActor)
{
	LightningCloudActor->OnStruckLightning.RemoveDynamic(this, &ALightningCloudsManager::LightningStrucked);
	ActiveLightningClouds.Remove(LightningCloudActor);
	FinishedLightning++;
	if (FinishedLightning == NbLightning)
	{
		OnEventEnded.Broadcast(this);
	}
}

void ALightningCloudsManager::CreateLightningCloud()
{
	FTransform SpawnTransform = GetTransform();
	SpawnTransform.SetLocation(GetRandomLightningCloudPosition());
	
	ALightningCloud* LightningCloud = GetWorld()->SpawnActorDeferred<ALightningCloud>(LightningCloudClass, SpawnTransform);
	
	LightningCloud->SetupLightningCloud(
		FMath::RandRange(LightningMinDuration, LightningMaxDuration),
		FMath::RandRange(LightningMinRadius, LightningMaxRadius));

	LightningCloud->OnStruckLightning.AddDynamic(this, &ALightningCloudsManager::LightningStrucked);
	LightningCloud->FinishSpawning(SpawnTransform);
	ActiveLightningClouds.Add(LightningCloud);
}

FVector ALightningCloudsManager::GetRandomLightningCloudPosition()
{
	FVector SpawnPosition = FVector::ZeroVector;
	int nbIterations = 0;
	do
	{
		SpawnPosition = FVector(
			FMath::RandRange(-SpawningRange.X, SpawningRange.X),
			FMath::RandRange(-SpawningRange.Y, SpawningRange.Y),
			0);
		SpawnPosition += GetActorLocation();
		nbIterations++;
	} while (IsNearAnyClouds(SpawnPosition) && nbIterations <= 100000);

	if (nbIterations >= 10000) GEngine->AddOnScreenDebugMessage(-1, 100.f, FColor::Red, "Couldn't find any free cloud position");
	return SpawnPosition;
}

bool ALightningCloudsManager::IsNearAnyClouds(const FVector& SpawnPosition)
{
	if(ActiveLightningClouds.Num() <= 0) return false;
	for (TObjectPtr<ALightningCloud> LightningCloud : ActiveLightningClouds)
	{
		float Distance = FVector::Dist(SpawnPosition, LightningCloud->GetActorLocation());
		if (Distance < LightningCloud->LightningRadius + MinDistanceBetweenClouds)
		{
			return true;
		}
	}

	return false;
}

