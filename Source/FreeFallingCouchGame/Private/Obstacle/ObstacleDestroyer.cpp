// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleDestroyer.h"

#include "Components/BoxComponent.h"
#include "Obstacle/Obstacle.h"


// Sets default values
AObstacleDestroyer::AObstacleDestroyer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>("Collision");
	RootComponent = BoxCollision;

	DrawDebugBox(GetWorld(), GetTransform().GetLocation(), BoxCollision->GetScaledBoxExtent(), FColor::Red, false, 99999, 0,1	);
}

void AObstacleDestroyer::BeginPlay()
{
	Super::BeginPlay();
	DrawDebugBox(GetWorld(), GetTransform().GetLocation(), BoxCollision->GetScaledBoxExtent(), FColor::Red, true, -1, 0,1	);
	
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AObstacleDestroyer::OnBeginOverlap);
	
}

void AObstacleDestroyer::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AObstacle* Obstacle = (AObstacle*)OtherActor;
	if(Obstacle == nullptr) return;

	Obstacle->Destroy();
}
