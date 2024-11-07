// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GrabbableInterface.h"
#include "PowerUpCollectible.generated.h"

class UPowerUpObject;
enum class EPowerUpsID : uint8;

UCLASS()
class FREEFALLINGCOUCHGAME_API APowerUpCollectible : public AActor, public IGrabbableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerUpCollectible();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual bool CanBeGrabbed() override;
	
	virtual bool CanBeTaken() override;

	virtual void Use(AFreeFallCharacter* Character) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPowerUpObject> PowerUpObject;

	//TODO: Move this in spawner
	/*
	UPROPERTY(EditAnywhere)
	TMap<EPowerUpsID, TSubclassOf<UPowerUpObject>> PowerUpComponentClasses;*/
};
