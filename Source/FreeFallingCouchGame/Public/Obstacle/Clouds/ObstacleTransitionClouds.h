// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleTransitionClouds.generated.h"

class AFreeFallCharacter;

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleTransitionClouds : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacleTransitionClouds();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceTolerance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpawnExtent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayAfterStartTransition;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayBeforeEndTransition;
public:
	UFUNCTION(BlueprintCallable)
	void StartTransition();
	UFUNCTION(BlueprintCallable)
	void EndTransition();
	UFUNCTION(BlueprintCallable, CallInEditor)
	void RandomizePlayersPositions();
	UFUNCTION(BlueprintCallable)
	bool IsNearAnyPlayer(const FVector& SpawnPosition, float Tolerance, TArray<FVector>& SpawnedPositionList);
};
