// Fill out your copyright notice in the Description page of Project Settings.


#include "Other/DiveLevels.h"

#include "Other/DiveLayersID.h"


// Sets default values
ADiveLevels::ADiveLevels()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ADiveLevels::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADiveLevels::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ADiveLevels::GetDiveBoundZCoord(EDiveLayersID Layer, EDiveLayerBoundsID Bound)
{
	float BoundMiddleCoordZ =
		Layer==EDiveLayersID::Top ? DiveLevelSize.Z * 2 + 1 :
		Layer==EDiveLayersID::Bottom ? DiveLevelSize.Z * -2 - 1 :
		0;

	BoundMiddleCoordZ+=
		Bound==EDiveLayerBoundsID::Up ? DiveLevelSize.Z/2 :
		Bound==EDiveLayerBoundsID::Down ? -DiveLevelSize.Z/2 :
		0;

	return GetActorLocation().Z + BoundMiddleCoordZ;
}

EDiveLayersID ADiveLevels::GetDiveLayersFromCoord(float PlayerCoordZ)
{
	if (PlayerCoordZ > GetDiveBoundZCoord(EDiveLayersID::Top, EDiveLayerBoundsID::Down))
	{
		return EDiveLayersID::Top;
	}
	if (PlayerCoordZ < GetDiveBoundZCoord(EDiveLayersID::Bottom,  EDiveLayerBoundsID::Up))
	{
		return EDiveLayersID::Bottom;
	}
	return EDiveLayersID::Middle;
}


