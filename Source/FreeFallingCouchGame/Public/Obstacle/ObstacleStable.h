// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Obstacle.h"
#include "ObstacleStable.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleStable : public AObstacle
{
	GENERATED_BODY()

#pragma region Unreal Properties
	
public:
	// Sets default values for this actor's properties
	AObstacleStable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma endregion

public:
	UPROPERTY(EditAnywhere, Category="Obstacle Stable")
	float StableHeight = 0.0f;

	UPROPERTY(EditAnywhere, Category="Obstacle Stable")
	float HeightTolerance = 10.0f;

	UPROPERTY(EditAnywhere, Category="Obstacle Stable")
	float StableTime = 5.0f;
	
	UPROPERTY()
	FTimerHandle StableTimerHandle;
	
	bool bHasBeenStable = false;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void EventBecommingStable();
};
