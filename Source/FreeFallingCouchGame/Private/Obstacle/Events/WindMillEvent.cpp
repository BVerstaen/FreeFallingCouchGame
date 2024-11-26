// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Events/WindMillEvent.h"


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

