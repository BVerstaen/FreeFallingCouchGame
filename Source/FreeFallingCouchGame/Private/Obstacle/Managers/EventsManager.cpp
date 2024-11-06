﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Managers/EventsManager.h"

#include "Obstacle/ObstacleSpawner.h"
#include "Obstacle/ObstacleSpawnerManager.h"
#include "Obstacle/Events/EventActor.h"


// Sets default values
AEventsManager::AEventsManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEventsManager::BeginPlay()
{
	Super::BeginPlay();

	EventClock = 0.f;

	for (AEventActor* EventActor : EventsActors)
	{
		AvailableEventsActors.Add(EventActor);
	}
}

void AEventsManager::Destroyed()
{
	Super::Destroyed();

	if (EventHappening && CurrentEventActor != nullptr)
	{
		CurrentEventActor->OnEventEnded.RemoveDynamic(this, &AEventsManager::OnEventEnded);
	}
}

// Called every frame
void AEventsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EventsActors.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,"No Events Set");
		return;
	}
	if (ObstacleSpawnerManager==nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,"EventManager missing reference to ObstacleSpawnerManager");
		return;
	}

	if (EventHappening) return;
	
	EventClock += DeltaTime;
	if (EventClock > FMath::Max(TimeBetweenEvents - ObstacleStopDifferenceTime,0) && ObstacleSpawnerManager->IsTimerPlaying())
	{
		ObstacleSpawnerManager->PauseTimer();
	}
	if (EventClock > TimeBetweenEvents)
	{
		//LaunchEvent
		AEventActor* EventActor = GetRandomEvent();
		if (EventActor == nullptr) return;
		EventActor->OnEventEnded.AddDynamic(this, &AEventsManager::OnEventEnded);
		EventActor->TriggerEvent();
		CurrentEventActor = EventActor;
		EventHappening = true;
		if (ObstacleSpawnerManager->IsTimerPlaying()) ObstacleSpawnerManager->PauseTimer();
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,"Trigger Event");
	}
}

void AEventsManager::OnEventEnded(AEventActor* TriggeringActor)
{
	TriggeringActor->OnEventEnded.RemoveDynamic(this, &AEventsManager::OnEventEnded);
	CurrentEventActor = nullptr;
	EventHappening = false;
	EventClock = 0.f;
	ObstacleSpawnerManager->ResumeTimer();
}

AEventActor* AEventsManager::GetRandomEvent()
{
	if (AvailableEventsActors.IsEmpty())
	{
		for (AEventActor* EventActor : EventsActors)
		{
			AvailableEventsActors.Add(EventActor);
		}
	}
	AEventActor* EventActor = AvailableEventsActors[FMath::RandRange(0, AvailableEventsActors.Num() - 1)];
	AvailableEventsActors.Remove(EventActor);
	return EventActor;
}


