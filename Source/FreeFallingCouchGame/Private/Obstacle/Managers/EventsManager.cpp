// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Managers/EventsManager.h"

#include "LocalizationDescriptor.h"
#include "GameMode/FreeFallGameMode.h"
#include "Haptic/HapticsStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Obstacle/ObstacleSpawner.h"
#include "Obstacle/ObstacleSpawnerManager.h"
#include "Obstacle/Events/CameraMoverActor.h"
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

	//Start Timer after round start
	if(AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		//TimeBetweenEvents = GameMode->GetCurrentParameters()->getTimerEventDelay();
		GameMode->OnStartRound.AddDynamic(this, &AEventsManager::OnStartTick);
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

void AEventsManager::OnStartTick()
{
	CanTickTimer = true;
}

bool AEventsManager::AtLeastOneObstacleSpawnerManagerPlaysTimer()
{
	bool isPlayingTimer = false;
	for(AObstacleSpawnerManager* Manager : ObstacleSpawnerManagerList)
	{
		isPlayingTimer = Manager->IsTimerPlaying();

		if(isPlayingTimer)
			break;
	}
	return isPlayingTimer;
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
	if (ObstacleSpawnerManagerList.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,"EventManager missing reference to ObstacleSpawnerManager");
		return;
	}

	if (EventHappening || !CanTickTimer) return;
	
	EventClock += DeltaTime;
	if (EventClock > FMath::Max(TimeBetweenEvents - ObstacleStopDifferenceTime,0) && AtLeastOneObstacleSpawnerManagerPlaysTimer())
	{
		for(AObstacleSpawnerManager* Manager : ObstacleSpawnerManagerList)
		{
			Manager->PauseTimer();
		}
	}
	if (EventClock > TimeBetweenEvents)
	{
		//LaunchEvent
		AEventActor* EventActor = GetRandomEvent();
		if (EventActor == nullptr) return;
		EventActor->OnEventEnded.AddDynamic(this, &AEventsManager::OnEventEnded);
		EventActor->TriggerEvent();
		CurrentEventActor = EventActor;
		UHapticsStatics::CallHapticsAll(this);
		EventHappening = true;
		if (AtLeastOneObstacleSpawnerManagerPlaysTimer())
		{
			for(AObstacleSpawnerManager* Manager : ObstacleSpawnerManagerList)
			{
				Manager->PauseTimer();
			}
		}
		if (CameraMoverActor != nullptr) CameraMoverActor->StopCameraManMovements();
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,"Trigger Event");
	}
}

void AEventsManager::OnEventEnded(AEventActor* TriggeringActor)
{
	TriggeringActor->OnEventEnded.RemoveDynamic(this, &AEventsManager::OnEventEnded);
	CurrentEventActor = nullptr;
	EventHappening = false;
	EventClock = 0.f;
	
	for(AObstacleSpawnerManager* Manager : ObstacleSpawnerManagerList)
	{
		Manager->ResumeTimer();
	}
	if (CameraMoverActor != nullptr) CameraMoverActor->StartCameraMovement();
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


