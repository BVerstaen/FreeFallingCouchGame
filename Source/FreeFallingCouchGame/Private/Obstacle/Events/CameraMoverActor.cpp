// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Events/CameraMoverActor.h"

#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACameraMoverActor::ACameraMoverActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACameraMoverActor::BeginPlay()
{
	Super::BeginPlay();

	//Start Timer after round start
	if(AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
	{
		GameMode->OnStartRound.AddDynamic(this, &ACameraMoverActor::StartCameraMovements);
	}
}

// Called every frame
void ACameraMoverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACameraMoverActor::TriggerEvent()
{
	Super::TriggerEvent();
}

void ACameraMoverActor::StartCameraMovements()
{
	RecieveStartCameraMovements();
}

void ACameraMoverActor::CallOnEventEnded()
{
	OnEventEnded.Broadcast(this);
}

