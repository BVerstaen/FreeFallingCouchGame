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

	//Attendre 1 seconde que le gamemode s'initialise A CHANGER PAR PITIE
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AArenaActor::Init, 1, false);
}

void AArenaActor::Init()
{
	//Set existing characters
	AFreeFallGameMode* FreeFallGameMode = Cast<AFreeFallGameMode>(GetWorld()->GetAuthGameMode());
	
	if(FreeFallGameMode->CharactersInsideArena.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "T niqué");
	CharactersInsideArena.Empty();
	CharactersInsideArena.Append(FreeFallGameMode->CharactersInsideArena);

	//Set off-screen tolerance
	const UCharactersSettings* CharactersSettings = GetDefault<UCharactersSettings>();
	OffScreenTolerance = CharactersSettings->DelayAliveOffScreen;
}

// Called every frame
void AArenaActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AFreeFallCharacter*> CharactersToRemove;
	//Check if character was rendered on screen
	for(AFreeFallCharacter* Character : CharactersInsideArena)
	{
		if(Character == nullptr) continue;
		
		if(!Character->WasRecentlyRendered(OffScreenTolerance))
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Character->GetName() + "is out");

			//Destroy current player
			if(OnCharacterDestroyed.IsBound())
				OnCharacterDestroyed.Broadcast(Character);
			
			CharactersToRemove.Add(Character);
			Character->Destroy();
		}
	}

	//Check if there's any characters to remove
	if(CharactersToRemove.Num() > 0)
	{
		for(AFreeFallCharacter* Character : CharactersToRemove)
			CharactersInsideArena.Remove(Character);
		CharactersToRemove.Empty();
	}
}
