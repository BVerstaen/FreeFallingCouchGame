// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BounceableInterface.generated.h"

class AFreeFallCharacter;

UENUM()
enum EBounceParameters
{
	Obstacle,
	Player
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UBounceableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FREEFALLINGCOUCHGAME_API IBounceableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FVector GetVelocity() = 0;
	virtual float GetMass() = 0;
	virtual EBounceParameters GetBounceParameterType() = 0;
	
	virtual void AddBounceForce(FVector Velocity);

	virtual AFreeFallCharacter* CollidedWithPlayer();
};
