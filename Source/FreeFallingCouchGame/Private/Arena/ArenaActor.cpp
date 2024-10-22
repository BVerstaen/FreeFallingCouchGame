// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/ArenaActor.h"

#include "Characters/FreeFallCharacter.h"
#include "GameMode/FreeFallGameMode.h"
#include "Settings/CharactersSettings.h"


// Sets default values
AArenaActor::AArenaActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OffScreenTolerance = 0.f;
}

// Called when the game starts or when spawned
void AArenaActor::BeginPlay()
{
	Super::BeginPlay();

	//Set existing characters
	AFreeFallGameMode* FreeFallGameMode = Cast<AFreeFallGameMode>(GetWorld()->GetAuthGameMode());
	CharactersInsideArena = FreeFallGameMode->CharactersInsideArena;

	//Set off-screen tolerance
	const UCharactersSettings* CharactersSettings = GetDefault<UCharactersSettings>();
	OffScreenTolerance = CharactersSettings->DelayAliveOffScreen;
}

// Called every frame
void AArenaActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Check if character was rendered on screen
	for(AFreeFallCharacter* Character : CharactersInsideArena)
	{
		if(Character == nullptr) continue;
		if(!Character->WasRecentlyRendered(OffScreenTolerance))
		{
			//Call death function
		}
	}
}

