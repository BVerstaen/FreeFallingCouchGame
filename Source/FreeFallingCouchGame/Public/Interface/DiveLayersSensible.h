// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DiveLayersSensible.generated.h"

enum class EDiveLayersID : uint8;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UDiveLayersSensible : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FREEFALLINGCOUCHGAME_API IDiveLayersSensible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ApplyDiveForce(FVector DiveForceDirection, float DiveStrength);

	virtual FVector GetDivingVelocity();

	//If true, player will receive DiveForces
	virtual bool IsDiveForced();

	//If true, player will be Bounded by its BoundedLayer;
	virtual bool IsBoundedByLayer();

	//Layer the player has to stay inside, If BoundedLayer is LayerNone, Actor is Bounded to the Top of Up DiveLayer and Down of Bottom DiveLayer
	virtual EDiveLayersID GetBoundedLayer();

	virtual AActor* GetSelfActor();
};
