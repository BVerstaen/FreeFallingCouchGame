// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GrabbableInterface.h"
#include "PowerUpCollectible.generated.h"

class UPowerUpObject;
enum class EPowerUpsID : uint8;

UCLASS()
class FREEFALLINGCOUCHGAME_API APowerUpCollectible : public AActor
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

	UFUNCTION(BlueprintCallable)
	void GivePowerToCharacter(AFreeFallCharacter* Character);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDestruction);
	FOnDestruction OnDestructionEvent;
	
	UFUNCTION(BlueprintCallable)
	void RemoveCharacterFromOverlappingCharacters(AFreeFallCharacter* Character);

	//For Game Programming
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPowerUpObject> PowerUpObject;

	//Lifetime of PowerUps Collectibles
	UPROPERTY(EditAnywhere)
	float LifeTime = 10.0f;

	UPROPERTY()
	TObjectPtr<AFreeFallCharacter> OverlappingCharacter;

	private:
	float LifeClock = 0.f;
};
