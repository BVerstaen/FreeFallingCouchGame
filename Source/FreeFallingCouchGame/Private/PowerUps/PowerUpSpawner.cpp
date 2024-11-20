// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpSpawner.h"

#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "PowerUps/PowerUpCollectible.h"
#include "PowerUps/PowerUpSpawnPoint.h"


// Sets default values
APowerUpSpawner::APowerUpSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APowerUpSpawner::BeginPlay()
{
	Super::BeginPlay();

	//Start Timer after round start
	if(AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnStartRound.AddDynamic(this, &APowerUpSpawner::StartTimer);
	}
}

// Called every frame
void APowerUpSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APowerUpSpawner::StartTimer()
{
	GetWorldTimerManager().SetTimer(PowerUpSpawnTimer, this, &APowerUpSpawner::SpawnPowerUps, GetSpawnCooldown() ,false);
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

void APowerUpSpawner::FindPowerUpSpawnPoints()
{
	SpawnPointActors.Empty();
	TArray<TObjectPtr<AActor>> ActorsFound;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerUpSpawnPoint::StaticClass(), ActorsFound);
	for (TObjectPtr<AActor> ActorFound : ActorsFound)
	{
		TObjectPtr<APowerUpSpawnPoint> SpawnPointActor = Cast<APowerUpSpawnPoint>(ActorFound);
		if (SpawnPointActor != nullptr) SpawnPointActors.Add(SpawnPointActor);
	}
}

float APowerUpSpawner::GetSpawnCooldown() const
{
	return FMath::RandRange(MinSpawnCooldown, MaxSpawnCooldown);
}

void APowerUpSpawner::SpawnPowerUps()
{
	TArray<EPowerUpsID> AvailablePowerUpsIds;
	PowerUpComponentClasses.GenerateKeyArray(AvailablePowerUpsIds);
	if (AvailablePowerUpsIds.Num() > 0 && HasValidSpawnLocation())
	{
		FTransform SpawnTransform = GetTransform();
		TObjectPtr<APowerUpSpawnPoint> SpawnPoint = GetValidSpawnLocation();
		SpawnTransform.SetLocation(SpawnPoint->GetActorLocation());
		APowerUpCollectible* PowerUpCollectible = GetWorld()->SpawnActorDeferred<APowerUpCollectible>(PowerUpCollectibleClass,SpawnTransform);
		
		EPowerUpsID PowerUpId = AvailablePowerUpsIds[FMath::RandRange(0,AvailablePowerUpsIds.Num() - 1)];
		PowerUpCollectible->PowerUpObject = PowerUpComponentClasses[PowerUpId];
		
		UGameplayStatics::FinishSpawningActor(PowerUpCollectible, SpawnTransform);
		SpawnPoint->SetSpawnedPowerUp(PowerUpCollectible);
		
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Emerald,"Spawning PowerUp : " + UEnum::GetDisplayValueAsText(PowerUpId).ToString());
	}
	else
	{
		if (AvailablePowerUpsIds.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"No PowerUps Available in Spawner");
		else if (AvailablePowerUpsIds.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"No PowerUps Available in Spawner");
		else
		{
			GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"All Spawnpoints are occupied");
			RestartTimer();
			return;
		}
		StopTimer();
		return;
	}
	RestartTimer();
}

bool APowerUpSpawner::HasValidSpawnLocation()
{
	if (SpawnPointActors.Num() > 0)
	{
		for (TObjectPtr<APowerUpSpawnPoint> SpawnPoint : SpawnPointActors)
		{
			if (!SpawnPoint->IsOccupied()) return true;
		}
		return false;
	}
	return false;
}

TObjectPtr<APowerUpSpawnPoint> APowerUpSpawner::GetValidSpawnLocation()
{
	TObjectPtr<APowerUpSpawnPoint> SpawnPoint;
	do
	{
		SpawnPoint = SpawnPointActors[FMath::RandRange(0,SpawnPointActors.Num() - 1)];
	} while (SpawnPoint->IsOccupied());
	return SpawnPoint;
}

