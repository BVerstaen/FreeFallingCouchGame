// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FreeFallGameMode.h"

#include "Characters/FreeFallCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/MapSettings.h"

void AFreeFallGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<APlayerStart*> PlayerStartsPoints;
	FindPlayerStartActorsInMap(PlayerStartsPoints);
	SpawnCharacters(PlayerStartsPoints);
	
}

void AFreeFallGameMode::FindPlayerStartActorsInMap(TArray<APlayerStart*>& ResultsActors)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);

	for (int i = 0; i < FoundActors.Num(); ++i)
	{
		APlayerStart* PlayerStartActor = Cast<APlayerStart>(FoundActors[i]);
		
		if (PlayerStartActor == nullptr) continue;
		
		ResultsActors.Add(PlayerStartActor);
	}
}

TSubclassOf<AFreeFallCharacter> AFreeFallGameMode::GetFreeFallCharacterClassFromInputType(
	EAutoReceiveInput::Type InputType) const
{
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	
	switch (InputType)
	{
	case EAutoReceiveInput::Player0:
		return MapSettings->CharacterClassP0;
	case EAutoReceiveInput::Player1:
		return MapSettings->CharacterClassP1;
	case EAutoReceiveInput::Player2:
		return MapSettings->CharacterClassP2;
	case EAutoReceiveInput::Player3:
		return MapSettings->CharacterClassP3;
	default:
		return nullptr;
	}
}

void AFreeFallGameMode::SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints)
{
	for (APlayerStart* SpawnPoint : SpawnPoints)
	{
		EAutoReceiveInput::Type InputType = SpawnPoint->AutoReceiveInput.GetValue();
		TSubclassOf<AFreeFallCharacter> CharacterClass = GetFreeFallCharacterClassFromInputType(InputType);
		
		if (CharacterClass == nullptr) continue;

		AFreeFallCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<AFreeFallCharacter>(
			CharacterClass,
			SpawnPoint->GetTransform()
			);

		if (NewCharacter == nullptr) continue;
		
		NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());
		

		/*
		NewCharacter->InputData = InputData;
		NewCharacter->InputMappingContext = InputMappingContext;
		NewCharacter->SetOrientX(SpawnPoint->GetStartOrientX());*/

		CharactersInsideArena.Add(NewCharacter);
	}
}
