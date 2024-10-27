// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GrabbableInterface.generated.h"

class AFreeFallCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UGrabbableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FREEFALLINGCOUCHGAME_API IGrabbableInterface
{
	GENERATED_BODY()
	
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool CanBeGrabbed() = 0;

	virtual bool CanBeTaken() = 0;

	virtual void Use(AFreeFallCharacter* Character) {}
};
