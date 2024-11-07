// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerUpObject.generated.h"


enum class EPowerUpsID : uint8;
class AFreeFallCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FREEFALLINGCOUCHGAME_API UPowerUpObject : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPowerUpObject();

	virtual void Use();

	void SetupCharacter(AFreeFallCharacter* Character);

	virtual void PrepareForDestruction();

	virtual EPowerUpsID GetPowerUpID();

private:
	UPROPERTY()
	AFreeFallCharacter* OwnerCharacter = nullptr;
};
