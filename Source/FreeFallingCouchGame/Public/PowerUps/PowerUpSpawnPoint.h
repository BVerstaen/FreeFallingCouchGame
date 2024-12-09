// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpSpawnPoint.generated.h"

class APowerUpCollectible;
class ADiveLevels;

UCLASS()
class FREEFALLINGCOUCHGAME_API APowerUpSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APowerUpSpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<APowerUpCollectible> PowerUpCollectible;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	ADiveLevels* DiveLevelsActor;

	UFUNCTION(CallInEditor)
	void CenterOnCurrentDiveLevel();

	bool IsOccupied();
		
	void SetSpawnedPowerUp(TObjectPtr<APowerUpCollectible> PowerUp);

	UFUNCTION()
	void PowerUpCollected();
	
};
