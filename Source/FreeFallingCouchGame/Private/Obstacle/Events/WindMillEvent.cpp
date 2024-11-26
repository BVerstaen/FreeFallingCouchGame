// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Events/WindMillEvent.h"

#include "Obstacle/ObstacleSpawnerManager.h"


// Sets default values
AWindMillEvent::AWindMillEvent()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWindMillEvent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWindMillEvent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWindMillEvent::TriggerEvent()
{
	Super::TriggerEvent();

	//Deactivate every obstacle manager
	for(AObstacleSpawnerManager* Manager : ObstaclesToDisable)
	{
		Manager->PauseTimer();
	}
	
	WindMillSpawner->bPlaySpawnTimer = false;
	AObstacle* SpawnedWindMill = WindMillSpawner->SpawnObstacle();
	if(!SpawnedWindMill)
	{
		StopEvent(nullptr);
		return;
	}
	SpawnedWindMill->OnDestroyed.AddDynamic(this, &AWindMillEvent::StopEvent);
}

void AWindMillEvent::StopEvent(AActor* DestroyedActor)
{
	if(DestroyedActor)
	{
		if(DestroyedActor->OnDestroyed.IsAlreadyBound(this, &AWindMillEvent::StopEvent))
		{
			DestroyedActor->OnDestroyed.RemoveDynamic(this, &AWindMillEvent::StopEvent);
		}
	}
	
	//Reactivate every obstacle manager
	for(AObstacleSpawnerManager* Manager : ObstaclesToDisable)
	{
		Manager->ResumeTimer();
	}
	
	OnEventEnded.Broadcast(this);
}

