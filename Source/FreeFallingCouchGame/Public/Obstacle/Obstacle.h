// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacle : public AActor
{
	GENERATED_BODY()

#pragma region Unreal Properties
	
public:
	// Sets default values for this actor's properties
	AObstacle();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
#pragma endregion

#pragma region Parameters

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> Layer1Material;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> Layer2Material;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> Layer3Material;
	
	/*Sera modifiée par l'Obstacle Spawner*/
	UPROPERTY(VisibleAnywhere)
	float Speed;

	/*Sera modifiée par l'Obstacle Spawner*/
	UPROPERTY(VisibleAnywhere)
	FVector Direction;

#pragma endregion
};
