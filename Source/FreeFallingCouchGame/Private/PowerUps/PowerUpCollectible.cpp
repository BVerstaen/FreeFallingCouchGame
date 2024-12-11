// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUps/PowerUpCollectible.h"

#include "Audio/SoundSubsystem.h"
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
	LifeClock += DeltaTime;
	if (LifeClock >= LifeTime)
	{
		OnDestructionEvent.Broadcast();
		Destroy();
		return;
	}
	if (OverlappingCharacter != nullptr)
	{
		if (OverlappingCharacter->CurrentPowerUp == nullptr)
		{
			GivePowerToCharacter(OverlappingCharacter);
			RemoveCharacterFromOverlappingCharacters(OverlappingCharacter);
		}
	}
}

void APowerUpCollectible::GivePowerToCharacter(AFreeFallCharacter* Character)
{
	if (PowerUpObject == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(
		-1, 5, FColor::Red,
		TEXT("Can't collect PowerUp Collectible, UObject is nullptr")
		);
		return;
	}

	if (Character->CurrentPowerUp != nullptr)
	{
		OverlappingCharacter = Character;
		return;
	}
	
	UPowerUpObject* CreatedPowerUpObj = NewObject<UPowerUpObject>(this, *PowerUpObject);
	CreatedPowerUpObj->SetupCharacter(Character);
	Character->SetPowerUp(CreatedPowerUpObj);

	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_GPE_Power-up_ST", this, false);
	if(Character)
	{
		SoundSubsystem->PlaySound("VOC_PLR_power-up_ST", Character, true);
		Character->PlayVoiceSound("GetPowerUp");
	}
	
	GEngine->AddOnScreenDebugMessage(
		-1, 5, FColor::Emerald,
		TEXT("Player : " + Character->GetSelfActor()->GetName() + " Collected PowerUp : " + UEnum::GetDisplayValueAsText(CreatedPowerUpObj->GetPowerUpID()).ToString())
		);
	OnDestructionEvent.Broadcast();
	
	this->Destroy();
}

void APowerUpCollectible::RemoveCharacterFromOverlappingCharacters(AFreeFallCharacter* Character)
{
	if (Character == OverlappingCharacter)
	{
		OverlappingCharacter = nullptr;
	}
}

EPowerUpsID APowerUpCollectible::GetPowerUpID()
{
	return PowerUpID;
}

void APowerUpCollectible::SetPowerUp(EPowerUpsID ID, TSubclassOf<UPowerUpObject> Object)
{
	PowerUpObject = Object;
	PowerUpID = ID;
}

