// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Obstacle.h"


// Sets default values
AObstacle::AObstacle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//Create Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	
	//Initialize parameters
	Speed = 0.0f;
	Direction = FVector::ZeroVector;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	

	Direction.Normalize();
	FVector ImpulseVector = Direction * Speed;
	Mesh->AddImpulse(ImpulseVector, NAME_None, true);
}

UStaticMeshComponent* AObstacle::GetMesh()
{
	return Mesh;
}

void AObstacle::ResetLaunch()
{
	Mesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
	Direction.Normalize();
	FVector ImpulseVector = Direction * Speed;
	Mesh->AddImpulse(ImpulseVector, NAME_None, true);
}
