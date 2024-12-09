// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventActor.h"
#include "Obstacle/ObstacleSpawnerManager.h"
#include "DoubleSpawnEvent.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API ADoubleSpawnEvent : public AEventActor
{
	GENERATED_BODY()
	
#pragma region Unreal Properties
	
public:
	// Sets default values for this actor's properties
	ADoubleSpawnEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
#pragma endregion

#pragma region Parameters

public:
	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AObstacleSpawnerManager>> SpawnerManagers;
	
#pragma endregion 
	
#pragma region Events

public:
	virtual void TriggerEvent() override;
	
#pragma endregion
};
