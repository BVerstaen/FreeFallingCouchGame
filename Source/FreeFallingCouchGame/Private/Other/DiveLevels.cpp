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

float ADiveLevels::GetDiveBoundZCoord(EDiveLayersID Layer, bool UpBound)
{
	float BoundZ = -1.f;
	switch (Layer)
	{
	case EDiveLayersID::Top:
		BoundZ = DiveLevelSize.Z * 2 + 1 + (DiveLevelSize.Z/2 * UpBound? 1 : -1); 
		break;
		
	case EDiveLayersID::Middle:
		BoundZ = UpBound? DiveLevelSize.Z/2 : -DiveLevelSize.Z/2;
		break;
		
	case EDiveLayersID::Down:
		BoundZ = (DiveLevelSize.Z * 2 + 1) * -1 + (DiveLevelSize.Z/2 * UpBound? 1 : -1); 
		break;

	default:
		BoundZ = -1.f;
	}

	return GetActorLocation().Z + BoundZ;
}

EDiveLayersID ADiveLevels::GetDiveLayersFromCoord(float PlayerCoordZ)
{
	if (PlayerCoordZ > GetDiveBoundZCoord(EDiveLayersID::Middle, true))
	{
		return EDiveLayersID::Top;
	}
	if (PlayerCoordZ < GetDiveBoundZCoord(EDiveLayersID::Middle, false))
	{
		return EDiveLayersID::Down;
	}
	return EDiveLayersID::Middle;
}


