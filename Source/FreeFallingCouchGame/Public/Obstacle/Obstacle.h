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
	
public :
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(VisibleAnywhere)
	float Speed;

	UPROPERTY(VisibleAnywhere)
	FVector Direction;

#pragma endregion
};
