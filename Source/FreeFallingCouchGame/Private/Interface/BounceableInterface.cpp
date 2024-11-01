// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/BounceableInterface.h"


// Add default functionality here for any IBouncableInterface functions that are not pure virtual.
void IBounceableInterface::AddBounceForce(FVector Velocity)
{
}

AFreeFallCharacter* IBounceableInterface::CollidedWithPlayer()
{
	return nullptr;
}
