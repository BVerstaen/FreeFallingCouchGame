// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpSpawnPoint.h"

#include "Kismet/GameplayStatics.h"
#include "Other/DiveLayersID.h"
#include "Other/DiveLevels.h"
#include "PowerUps/PowerUpCollectible.h"


// Sets default values
APowerUpSpawnPoint::APowerUpSpawnPoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APowerUpSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APowerUpSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APowerUpSpawnPoint::CenterOnCurrentDiveLevel()
{
	if (DiveLevelsActor==nullptr) DiveLevelsActor = Cast<ADiveLevels>(UGameplayStatics::GetActorOfClass(GetWorld(), ADiveLevels::StaticClass()));
	if (DiveLevelsActor!=nullptr)
	{
		FVector ActorLocation = GetActorLocation();
		float DiveBoundZCoord = DiveLevelsActor->GetDiveBoundZCoord(DiveLevelsActor->GetDiveLayersFromCoord(ActorLocation.Z), EDiveLayerBoundsID::Middle);
		SetActorLocation(FVector(ActorLocation.X, ActorLocation.Y, DiveBoundZCoord));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Red,"DiveLevelsActor isn't on Scene");
	}
}

bool APowerUpSpawnPoint::IsOccupied()
{
	return PowerUpCollectible != nullptr;
}

void APowerUpSpawnPoint::SetSpawnedPowerUp(TObjectPtr<APowerUpCollectible> PowerUp)
{
	PowerUpCollectible = PowerUp;
	PowerUpCollectible->OnDestructionEvent.AddDynamic(this, &APowerUpSpawnPoint::PowerUpCollected);
}

void APowerUpSpawnPoint::PowerUpCollected()
{
	if (PowerUpCollectible != nullptr) PowerUpCollectible->OnDestructionEvent.RemoveDynamic(this, &APowerUpSpawnPoint::PowerUpCollected);
	PowerUpCollectible = nullptr;
}

