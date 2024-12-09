// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventActor.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AEventActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEventActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void TriggerEvent();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventEnding, AEventActor*, TriggeringEventActor);
	FEventEnding OnEventEnded;
};
