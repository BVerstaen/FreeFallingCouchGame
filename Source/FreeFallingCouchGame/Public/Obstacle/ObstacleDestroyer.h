// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ObstacleDestroyer.generated.h"

class UBoxComponent;

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleDestroyer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacleDestroyer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> BoxCollision;
};
