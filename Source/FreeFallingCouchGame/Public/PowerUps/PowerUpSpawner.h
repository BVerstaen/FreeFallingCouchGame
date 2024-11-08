// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpSpawner.generated.h"

class APowerUpCollectible;
enum class EPowerUpsID : uint8;
class UPowerUpObject;

UCLASS()
class FREEFALLINGCOUCHGAME_API APowerUpSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerUpSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void PauseTimer();

	void ResumeTimer();

	void RestartTimer();

	void StopTimer();

protected:

	//Add SpawnPointActor's Location to Spawnpoints Lists
	UFUNCTION(CallInEditor)
	void ValidateSpawnPoint();

	//SpawnPointActor to setup Spawnpoint List, Variable is only used to fill List using ValidateSpawnPoint function, Select Actor in Scene
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AActor> SpawnPointActor;

private:
	float GetSpawnCooldown() const;
	
	void SpawnPowerUps();

	FTimerHandle PowerUpSpawnTimer;

	UPROPERTY(EditAnywhere)
	float MinSpawnCooldown;

	UPROPERTY(EditAnywhere)
	float MaxSpawnCooldown;

	//Can be set using Validate Spawnpoints
	UPROPERTY(EditAnywhere)
	TArray<FVector> SpawnPoints;

	//For Game Programming
	UPROPERTY(EditAnywhere)
	TMap<EPowerUpsID, TSubclassOf<UPowerUpObject>> PowerUpComponentClasses;

	//For Game Programming
	UPROPERTY(EditAnywhere)
	TSubclassOf<APowerUpCollectible> PowerUpCollectibleClass;
};
