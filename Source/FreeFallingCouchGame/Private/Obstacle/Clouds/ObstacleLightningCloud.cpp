// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Clouds/ObstacleLightningCloud.h"

#include "Audio/SoundSubsystem.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AObstacleLightningCloud::AObstacleLightningCloud()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObstacleLightningCloud::BeginPlay()
{
	Super::BeginPlay(); //<- Spawn des Lightnings
}

// Called every frame
void AObstacleLightningCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsLightning)
	{
		LightningClock+=DeltaTime;
		if (LightningClock >= LightningDuration)
		{
			bIsLightning = false;

			const AFreeFallGameMode* FreeFallGameMode = Cast<AFreeFallGameMode>(GetWorld()->GetAuthGameMode());
			if(FreeFallGameMode == nullptr) return;
			if(FreeFallGameMode->CharactersInsideArena.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No Existing Character in Arena");
	
			TArray<AFreeFallCharacter*> CharactersInArena;
			CharactersInArena.Empty();
			CharactersInArena.Append(FreeFallGameMode->CharactersInsideArena);
			APlayerController* MainCameraController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			FVector2D ScreenPos;
			for (AFreeFallCharacter* Character : CharactersInArena)
			{
				bool bCanConvertToScreen = MainCameraController->ProjectWorldLocationToScreen(Character->GetActorLocation(), ScreenPos);
				if(bCanConvertToScreen)
				{
					//Check if inside of Lightning
				}
			}
		}
	}
}

void AObstacleLightningCloud::TriggerEvent()
{
	Super::TriggerEvent();

	//Launch Event

	bIsLightning = true;
	LightningClock = 0.f;
	
	//Play sound
	USoundSubsystem* SoundSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_GPE_Thunder/Flash_ST", this, true);
}

