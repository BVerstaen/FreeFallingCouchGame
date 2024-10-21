// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "CameraHandler.generated.h"

enum class ECameraStates;

UCLASS()
class FREEFALLINGCOUCHGAME_API ACameraHandler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	ECameraStates CurrentCameraState;
	ACameraActor CurrentCamera;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TMap<ECameraStates, ACameraActor> CameraList;
};


//BlueprintType permet d’exposer l’enum pour créer des variables blueprint avec l’enum déclaré en dessous.
UENUM(BlueprintType)
enum class ECameraStates : uint8
{
	Disabled = 0,
	Base,
	Menu,
	Above,
	Front,
	Dynamic,
};