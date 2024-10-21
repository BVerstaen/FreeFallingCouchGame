// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraHandler.h"

#include "UObject/UnrealTypePrivate.h"

// Sets default values
ACameraHandler::ACameraHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACameraHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACameraHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

