// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/DiveLayersSensible.h"

#include "Other/DiveLayersID.h"


// Add default functionality here for any IDiveLayersSensible functions that are not pure virtual.
void IDiveLayersSensible::ApplyDiveForce(FVector DiveForceDirection, float DiveStrength)
{
}

FVector IDiveLayersSensible::GetDivingVelocity()
{
	return FVector(0, 0, 0);
}

bool IDiveLayersSensible::IsDiveForced()
{
	return true;
}

bool IDiveLayersSensible::IsBoundedByLayer()
{
	return true;
}

EDiveLayersID IDiveLayersSensible::GetBoundedLayer()
{
	return EDiveLayersID::None;
}

AActor* IDiveLayersSensible::GetSelfActor()
{
	return nullptr;
}
