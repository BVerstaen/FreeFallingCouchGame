// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Obstacle/Events/EventActor.h"
#include "ObstacleLightningCloud.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleLightningCloud : public AEventActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacleLightningCloud();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TriggerEvent() override;

private:
	UPROPERTY(EditAnywhere)
	float EventTime;

	UPROPERTY(EditAnywhere)
	int nbLightning;

	UPROPERTY(EditAnywhere)
	float LightningRadius;
};
