// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FreeFallGameMode.h"

#include "LocalMultiplayerSubsystem.h"
#include "Characters/FreeFallCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/CharactersSettings.h"
#include "Settings/MapSettings.h"

void AFreeFallGameMode::BeginPlay()
{
	Super::BeginPlay();
	//TODO Find way to receive player made modifications
	SetupMatch(nullptr);
	CreateAndInitsPlayers();

	TArray<APlayerStart*> PlayerStartsPoints;
	FindPlayerStartActorsInMap(PlayerStartsPoints);
	SpawnCharacters(PlayerStartsPoints);
}

void AFreeFallGameMode::CreateAndInitsPlayers() const
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance == nullptr) return;

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (LocalMultiplayerSubsystem == nullptr) return;

	LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::InGame);
}

UFreeFallCharacterInputData* AFreeFallGameMode::LoadInputDataFromConfig()
{
	const UCharactersSettings* CharacterSettings = GetDefault<UCharactersSettings>();
	if (CharacterSettings == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("CharactersSettings est nullptr"));
		return nullptr;
	}
	return CharacterSettings->InputData.LoadSynchronous();
}

UInputMappingContext* AFreeFallGameMode::LoadInputMappingContextFromConfig()
{
	const UCharactersSettings* CharacterSettings = GetDefault<UCharactersSettings>();
	if (CharacterSettings == nullptr) return nullptr;
	return CharacterSettings->InputMappingContext.LoadSynchronous();
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
	UFreeFallCharacterInputData* InputData = LoadInputDataFromConfig();
	UInputMappingContext* InputMappingContext = LoadInputMappingContextFromConfig();
	
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
		NewCharacter->InputData = InputData;
		NewCharacter->InputMappingContext = InputMappingContext;
		NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());

		/*NewCharacter->SetOrientX(SpawnPoint->GetStartOrientX());*/

		CharactersInsideArena.Add(NewCharacter);
	}
}
#pragma region Rounds

void AFreeFallGameMode::EndRound()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "EndRound");
	if(OnEndRound.IsBound())
	{
		OnEndRound.Broadcast();
	}
//	if(CurrentRound > CurrentParameters->getMaxRounds())
	if(CurrentRound > 5)
	{
		ShowResults();
	} else
	{
		StartRound();
	}
}

void AFreeFallGameMode::ShowResults()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "ShowResults");
	CurrentRound = 0;
	if(OnResults.IsBound())
	{
		OnResults.Broadcast();
	}
}

void AFreeFallGameMode::StartRound()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartRound");
	if(OnStartRound.IsBound())
	{
		OnStartRound.Broadcast();
		// void refGamemode->OnStartRound.AddDynamic(this, &UNameOfClassYouCallThisIn::NameOfFunctionToTrigger);
	}
	CurrentRound++;
}

void AFreeFallGameMode::RoundEventTimer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartTimer");
	GetWorld()->GetTimerManager().SetTimer(
		RoundTimerHandle,
		this,
		&AFreeFallGameMode::StartEvent,
//		CurrentParameters->getTimerDelay(),
		10.0f,
		true
		);
}

void AFreeFallGameMode::StartEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartEvent");
	//Here can be implemented a random function to start random events
}

void AFreeFallGameMode::SetupMatch(TSubclassOf<UMatchParameters> UserParameters)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "SetupMatch");
	// Get Values passed in selection screen
	if(IsValid(UserParameters))
	{
		CurrentParameters = NewObject<UMatchParameters>(UserParameters);

	} else
	{
		CurrentParameters = NewObject<UMatchParameters>(DefaultParameters);
		CurrentParameters->Init(DefaultParameters);
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "Test: " + CurrentParameters->getEraChosen());

	// Selection screen has a UMatchParameters variable and can call this when switching values
}
#pragma endregion
