// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Obstacle.h"
#include "UObject/Object.h"
#include "ObstacleSpawner.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleSpawner : public AActor
{
	GENERATED_BODY()

#pragma region Unreal Properties

public:
	// Sets default values for this actor's properties
	AObstacleSpawner();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma endregion
	
#pragma region Parameters
	
public :
	
	/* Liste des obstacles à faire spawn ( NE METTRE QUE DES "BP_" ) */
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AObstacle>> ObstaclesList;

	/* Taille de la zone où les obstacles peuvent spawner (visible en play mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector ObstacleSpawnExtant;
	
	/* La direction sera normalisé après donc mettre des nombres entre -1 & 1 */
	UPROPERTY(EditAnywhere)
	FVector ObstacleDirection;

	/* Vitesse minimum de l'obstacle */
	UPROPERTY(EditAnywhere)
	float ObstacleMinSpeed;

	/* Vitesse maximum de l'obstacle */
	UPROPERTY(EditAnywhere)
	float ObstacleMaxSpeed;

	/* Delay minimum entre 2 spawns */
	UPROPERTY(EditAnywhere)
	float ObstacleMinTimer;

	/* Delay maximum entre 2 spawns */
	UPROPERTY(EditAnywhere)
	float ObstacleMaxTimer;
	
#pragma endregion
	
private:
	FTimerHandle SpawnTimer;
	FVector GetRandomLocationSpawnVector() const;
	
public:
	void SpawnObstacle();
	
};
