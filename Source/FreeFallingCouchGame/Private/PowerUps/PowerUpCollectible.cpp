// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpCollectible.h"

#include "Characters/FreeFallCharacter.h"
#include "PowerUps/PowerUpObject.h"


// Sets default values
APowerUpCollectible::APowerUpCollectible()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APowerUpCollectible::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APowerUpCollectible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool APowerUpCollectible::CanBeGrabbed()
{
	return false;
}

bool APowerUpCollectible::CanBeTaken()
{
	return true;
}

void APowerUpCollectible::Use(AFreeFallCharacter* Character)
{
	UPowerUpObject* PowerUpObj = NewObject<UPowerUpObject>(this, *PowerUpObject);
	PowerUpObj->SetupCharacter(Character);
	Character->CurrentPowerUp->PrepareForDestruction();
	Character->CurrentPowerUp = PowerUpObj;
	
	GEngine->AddOnScreenDebugMessage(
		-1, 5, FColor::Emerald,
		TEXT("Player : " + Character->GetSelfActor()->GetName() + "Using PowerUpCollectible")
		);
}

