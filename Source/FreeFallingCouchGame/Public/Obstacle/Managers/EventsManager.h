// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventsManager.generated.h"

class AObstacleSpawnerManager;
class AEventActor;

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

	virtual void Destroyed() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	
private:
	AEventActor* GetRandomEvent();

	UFUNCTION()
	void OnEventEnded(AEventActor* TriggeringActor);

	UFUNCTION()
	void OnStartTick();
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<AObstacleSpawnerManager> ObstacleSpawnerManager = nullptr;

	//Temps entre chaque Events;
	UPROPERTY(EditAnywhere)
	float TimeBetweenEvents = 5.f;

	//Temps entre le Stoppage des Obstacles et L'Event (Les obstacles se stopperont x temps avant l'event)
	UPROPERTY(EditAnywhere)
	float ObstacleStopDifferenceTime = 0.f;

	UPROPERTY()
	float EventClock = 0.f;

	UPROPERTY()
	bool EventHappening = false;

	UPROPERTY()
	bool CanTickTimer = false;
	
	UPROPERTY()
	TObjectPtr<AEventActor> CurrentEventActor = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<AEventActor*> EventsActors;

	UPROPERTY()
	TArray<AEventActor*> AvailableEventsActors;
};
