// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiveLevels.generated.h"

enum class EDiveLayerBoundsID : uint8;
enum class EDiveLayersID : uint8;

UCLASS()
class FREEFALLINGCOUCHGAME_API ADiveLevels : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADiveLevels();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	float GetDiveBoundZCoord(EDiveLayersID Layer, EDiveLayerBoundsID Bound);

	float GetDiveSize();

	EDiveLayersID GetDiveLayersFromCoord(float PlayerCoordZ);


	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector3f DiveLevelSize;
};
