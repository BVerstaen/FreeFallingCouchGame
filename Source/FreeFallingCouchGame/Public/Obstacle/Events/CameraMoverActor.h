// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EventActor.h"
#include "CameraMoverActor.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API ACameraMoverActor : public AEventActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraMoverActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TriggerEvent() override;

	UFUNCTION()
	void StartCameraMovements();

	UFUNCTION(BlueprintCallable)
	void CallOnEventEnded();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "StartCameraMovement"))
	void RecieveStartCameraMovements();
};
