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
	void StartCameraMovement();
	
	void MoveCameraMan();

	void StopCameraManMovements();

	UFUNCTION(BlueprintCallable)
	void CallOnEventEnded();

	UFUNCTION(BlueprintCallable)
	void CallOnCameraManMovementEnded();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "Move CameraMan"))
	void ReceiveMoveCameraMan();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "Stop CameraMan"))
	void ReceiveStopCameraMan();

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName = "StartTopCamEvent"))
	void ReceiveTriggerEvent();

	FTimerHandle CameraManMovementTimer;

protected:
	UPROPERTY(EditAnywhere)
	float MinTimeBetweenMovements = 10.f;

	UPROPERTY(EditAnywhere)
	float MaxTimeBetweenMovements = 15.f;

	//True if Camera is not moving because an event is happening (CameraMover own event counts)
	UPROPERTY(BlueprintReadOnly)
	bool bIsOnEvent = false;

};
