// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleDestroyer.h"

#include "Components/BoxComponent.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
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
	
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &AObstacleDestroyer::OnOverlapEnd);

	//Bind Inside Destroyer to StartRound
	AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(GetWorld()->GetAuthGameMode());
	if(GameMode)
	{
		GameMode->OnStartRound.AddDynamic(this, &AObstacleDestroyer::DestroyObstacleInside);
	}
}

void AObstacleDestroyer::OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AObstacle* Obstacle = Cast<AObstacle>(OtherActor);
	if(Obstacle == nullptr) return;

	Obstacle->Destroy();
}

void AObstacleDestroyer::DestroyObstacleInside()
{
	TArray<TObjectPtr<AActor>> ObstaclesToDestroy = FindObstaclesInside();
	if(ObstaclesToDestroy.Num() <= 0) return;

	for(AActor* Obstacle : ObstaclesToDestroy)
	{
		Obstacle->Destroy();
	}
}

TArray<TObjectPtr<AActor>> AObstacleDestroyer::FindObstaclesInside()
{
	TArray<TObjectPtr<AActor>> ObstaclesToDestroy;
	FVector BoxLocation = GetTransform().GetLocation();
	FVector BoxExtent = BoxCollision->GetScaledBoxExtent();
	ETraceTypeQuery TraceChannel = UEngineTypes::ConvertToTraceType(ECC_WorldDynamic);
	TArray<AActor*> ignoreActors;
	TArray<FHitResult> HitResults;
	
	bool CanGrab = UKismetSystemLibrary::BoxTraceMulti(
														GetWorld(),
														BoxLocation,
														BoxLocation,
														BoxExtent,
														FRotator::ZeroRotator,
														TraceChannel,
														false,
														ignoreActors,
														EDrawDebugTrace::None,
														HitResults,
														true);

	
	//Has grabbed anything
	if (CanGrab)
	{
		//Check if find any obstacles
		for(FHitResult HitResult : HitResults)
		{
			if(Cast<AObstacle>(HitResult.GetActor()))
				ObstaclesToDestroy.Add(HitResult.GetActor());
		}
		
	}
	return ObstaclesToDestroy;
}
