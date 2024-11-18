// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Clouds/ObstacleLightningCloud.h"

#include "Audio/SoundSubsystem.h"


// Sets default values
AObstacleLightningCloud::AObstacleLightningCloud()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObstacleLightningCloud::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObstacleLightningCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstacleLightningCloud::TriggerEvent()
{
	Super::TriggerEvent();

	//Launch Event

	//Play sound
	USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_GPE_Thunder/Flash_ST", this, true);
}

