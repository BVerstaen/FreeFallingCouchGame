// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventsManager.generated.h"

class AEventActor;
class AObstacleSpawner;

UCLASS()
class FREEFALLINGCOUCHGAME_API AEventsManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEventsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
private:
	AEventActor* GetRandomEvent();

	UFUNCTION()
	void OnEventEnded(AEventActor* TriggeringActor);

	UPROPERTY(EditAnywhere)
	AObstacleSpawner* ObstacleSpawner = nullptr;

	UPROPERTY(EditAnywhere)
	float TimeBetweenEvents = 5.f;

	UPROPERTY()
	float EventClock = 0.f;

	UPROPERTY()
	bool EventHappening = false;

	UPROPERTY(EditAnywhere)
	TArray<AEventActor*> EventsActors;

	UPROPERTY()
	TArray<AEventActor*> AvailableEventsActors;
};
