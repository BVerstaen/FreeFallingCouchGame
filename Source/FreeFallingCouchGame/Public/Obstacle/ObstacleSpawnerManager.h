// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObstacleSpawner.h"
#include "GameFramework/Actor.h"
#include "ObstacleSpawnerManager.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleSpawnerManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacleSpawnerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bTimerPlaying;

public:
	UFUNCTION()
	void StartTimer();
	
	void LaunchSpawn();

	UFUNCTION(CallInEditor)
	void PauseTimer();

	UFUNCTION(CallInEditor)
	void ResumeTimer();

	bool IsTimerPlaying();

	/* Liste des spawners d'obstacles*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AObstacleSpawner*> LinkedObstacleSpawners;

	/* Delay minimum entre 2 spawns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleMinTimer;

	/* Delay maximum entre 2 spawns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ObstacleMaxTimer;
	
	UPROPERTY()
	FTimerHandle SpawnerTimer;
	
	UPROPERTY(EditAnywhere)
	bool bDoubleSpawn;
	
#pragma region Camera Activation
public:
	/* Sous quel index de position de la caméra dois-je m'activer*/
	UPROPERTY(EditAnywhere)
	TArray<int> CameraPositionIndex;

	void ActivateCamera(int NewCamPosition);
#pragma endregion
};
