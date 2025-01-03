﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle/Events/EventActor.h"
#include "ObstacleTransitionClouds.generated.h"

class AFreeFallCharacter;

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleTransitionClouds : public AEventActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacleTransitionClouds();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/*Distance minimum entre les joueurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DistanceTolerance;

	/*Distance minimum entre les joueurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector SpawnExtent;

	/*Delay de la transition avant le TP des joueurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayAfterStartTransition;

	/*Delay de la transition après le TP des joueurs*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DelayBeforeEndTransition;
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "CloudTransitionEvent")
	void OnTransitionStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "CloudTransitionEvent")
	void OnTransitionCompleted();
	
	UFUNCTION(BlueprintCallable)
	void StartTransition();
	UFUNCTION(BlueprintCallable)
	void EndTransition();
	UFUNCTION(BlueprintCallable, CallInEditor)
	void RandomizePlayersPositions();
	UFUNCTION(BlueprintCallable)
	bool IsNearAnyPlayer(const FVector& SpawnPosition, float Tolerance, TArray<FVector>& SpawnedPositionList);
	UFUNCTION()
	void GetAllNearObstacles(TArray<FVector>& SpawnedPositionList);
	virtual void TriggerEvent() override;
};
