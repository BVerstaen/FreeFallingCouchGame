// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "PowerUps/PowerUpCollectible.h"


// Sets default values
APowerUpSpawner::APowerUpSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnPointActor = nullptr;
}

// Called when the game starts or when spawned
void APowerUpSpawner::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(PowerUpSpawnTimer, this, &APowerUpSpawner::SpawnPowerUps, GetSpawnCooldown() ,false);
}

// Called every frame
void APowerUpSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APowerUpSpawner::PauseTimer()
{
	GetWorldTimerManager().PauseTimer(PowerUpSpawnTimer);
}

void APowerUpSpawner::ResumeTimer()
{
	GetWorldTimerManager().UnPauseTimer(PowerUpSpawnTimer);
}

void APowerUpSpawner::RestartTimer()
{
	GetWorldTimerManager().ClearTimer(PowerUpSpawnTimer);
	GetWorldTimerManager().SetTimer(PowerUpSpawnTimer, this, &APowerUpSpawner::SpawnPowerUps, GetSpawnCooldown() ,false);
}

void APowerUpSpawner::StopTimer()
{
	GetWorldTimerManager().ClearTimer(PowerUpSpawnTimer);
}

void APowerUpSpawner::ValidateSpawnPoint()
{
	if (!SpawnPointActor)
	{
		GEngine->AddOnScreenDebugMessage(-1,3,FColor::Red,"SpawnPointActor not set");
		return;
	}
	SpawnPoints.Add(SpawnPointActor->GetActorLocation());
}

float APowerUpSpawner::GetSpawnCooldown() const
{
	return FMath::RandRange(MinSpawnCooldown, MaxSpawnCooldown);
}

void APowerUpSpawner::SpawnPowerUps()
{
	TArray<EPowerUpsID> AvailablePowerUpsIds;
	PowerUpComponentClasses.GenerateKeyArray(AvailablePowerUpsIds);
	if (AvailablePowerUpsIds.Num() > 0 && SpawnPoints.Num() > 0)
	{
		FTransform SpawnTransform = GetTransform();
		SpawnTransform.SetLocation(SpawnPoints[FMath::RandRange(0, SpawnPoints.Num() - 1)]);
		APowerUpCollectible* PowerUpCollectible = GetWorld()->SpawnActorDeferred<APowerUpCollectible>(PowerUpCollectibleClass,SpawnTransform);
		
		EPowerUpsID PowerUpId = AvailablePowerUpsIds[FMath::RandRange(0,AvailablePowerUpsIds.Num() - 1)];
		PowerUpCollectible->PowerUpObject = PowerUpComponentClasses[PowerUpId];
		
		UGameplayStatics::FinishSpawningActor(PowerUpCollectible, SpawnTransform);
		
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"Spawning PowerUp : " + UEnum::GetDisplayValueAsText(PowerUpId).ToString());
	}
	else
	{
		if (AvailablePowerUpsIds.Num() > 0) GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"No PowerUps Available in Spawner");
		else GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"No PowerUps Available in Spawner");
		StopTimer();
		return;
	}
	RestartTimer();
}

