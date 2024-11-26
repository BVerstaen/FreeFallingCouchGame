// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventActor.h"
#include "Obstacle/ObstacleSpawner.h"
#include "WindMillEvent.generated.h"

class AObstacleSpawnerManager;

UCLASS()
class FREEFALLINGCOUCHGAME_API AWindMillEvent : public AEventActor
{
	GENERATED_BODY()

#pragma region Unreal Properties
	
public:
	// Sets default values for this actor's properties
	AWindMillEvent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma endregion


#pragma region Properties

public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<AObstacleSpawner> WindMillSpawner;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<AObstacleSpawnerManager>> ObstaclesToDisable;
	
#pragma endregion 
	
#pragma region Events

public:
	virtual void TriggerEvent() override;

	UFUNCTION()
	void StopEvent(AActor* DestroyedActor);
	
#pragma endregion
};
