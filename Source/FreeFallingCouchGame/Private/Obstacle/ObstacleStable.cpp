// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleStable.h"


// Sets default values
AObstacleStable::AObstacleStable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObstacleStable::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AObstacleStable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if(!bHasBeenStable)
	{
		//Check if reached stable zone
		if(GetActorLocation().Z > StableHeight - HeightTolerance && GetActorLocation().Z < StableHeight + HeightTolerance)
		{
			bHasBeenStable = true;
			Mesh->SetPhysicsLinearVelocity(FVector(0, 0, 0), false);
			EventBecommingStable();
			GetWorldTimerManager().SetTimer(StableTimerHandle, this,  &AObstacleStable::StopStable, StableTime);
		}
	}
}

void AObstacleStable::StopStable()
{
	EventEndStable();
	ResetLaunch();
}

bool AObstacleStable::CanBeGrabbed()
{
	//Deactivate stable timer if can be grabbed
	if(IsGrabbable)
	{
		bHasBeenStable = true;
		StableTimerHandle.Invalidate();
	}
	
	return Super::CanBeGrabbed();
}
