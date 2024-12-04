// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Obstacle/Events/EventActor.h"
#include "LightningCloudsManager.generated.h"

class ALightningCloud;

UCLASS()
class FREEFALLINGCOUCHGAME_API ALightningCloudsManager : public AEventActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALightningCloudsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TriggerEvent() override;

	UFUNCTION()
	void LightningStrucked(ALightningCloud* LightningCloudActor);

protected:
	UPROPERTY(EditAnywhere, Category = "Cloud Manager", BlueprintReadOnly)
	FVector2D SpawningRange;
	
private:

	void CreateLightningCloud();

	bool IsNearAnyClouds(const FVector& SpawnPosition);

	FVector GetRandomLightningCloudPosition();
	
	UPROPERTY(EditAnywhere, Category = "Lightning")
	int NbLightning;

	UPROPERTY(EditAnywhere, Category = "Lightning")
	TSubclassOf<ALightningCloud> LightningCloudClass;


	UPROPERTY(EditAnywhere, Category = "Lightning Duration")
	float LightningMinDuration;

	UPROPERTY(EditAnywhere, Category = "Lightning Duration")
	float LightningMaxDuration;
	
	UPROPERTY(EditAnywhere, Category = "Lightning Radius")
	float LightningMaxRadius;

	UPROPERTY(EditAnywhere, Category = "Lightning Radius")
	float LightningMinRadius;
	
	UPROPERTY(EditAnywhere, Category = "Lightning Spawn")
	float MinSpawnCooldown;

	UPROPERTY(EditAnywhere, Category = "Lightning Spawn")
	float MaxSpawnCooldown;
	
	UPROPERTY(EditAnywhere, Category = "Lightning Spawn")
	float MinDistanceBetweenClouds = 1.f;

	UPROPERTY()
	TArray<TObjectPtr<ALightningCloud>> ActiveLightningClouds;

	bool bIsSpawningClouds;

	float SpawnClock = 0.f;

	float SpawnCooldown = 0.f;

	int SpawnedLightning = 0;

	int FinishedLightning = 0;
	
};
