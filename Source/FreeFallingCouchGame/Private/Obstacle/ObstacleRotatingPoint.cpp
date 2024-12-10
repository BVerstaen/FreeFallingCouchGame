// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleRotatingPoint.h"

#include "Kismet/KismetMathLibrary.h"
#include "Obstacle/ObstacleSpawner.h"


// Sets default values
AObstacleRotatingPoint::AObstacleRotatingPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObstacleRotatingPoint::BeginPlay()
{
	Super::BeginPlay();

	if(LinkedObstacle == nullptr) return;
	FAttachmentTransformRules AttachementRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, EAttachmentRule::KeepWorld, false);
	LinkedObstacle->AttachToActor(this, AttachementRules);
}

// Called every frame
void AObstacleRotatingPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Add rotation
	FRotator DeltaRotation = FRotator(0, RotatingSpeed * DeltaTime, 0);
	AddActorLocalRotation(DeltaRotation);

	//Make sure Linked object look at point
	FRotator NewLookRotation = UKismetMathLibrary::FindLookAtRotation(LinkedObstacle->GetActorLocation(), GetActorLocation());
	LinkedObstacle->GetRootComponent()->SetRelativeRotation(NewLookRotation);

	//Change Linked spawner direction
	FVector Direction = GetActorLocation() - LinkedObstacle->GetActorLocation();
	Direction.Normalize();
	Direction = FVector(Direction.X,Direction.Y,Direction.Z);
	LinkedObstacle->ObstacleDirection = Direction;
	
}

