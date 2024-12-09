// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleRotatingPoint.generated.h"

class AObstacleSpawner;

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleRotatingPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacleRotatingPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AObstacleSpawner> LinkedObstacle;
	
	UPROPERTY(EditAnywhere)
	float RotatingSpeed;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
