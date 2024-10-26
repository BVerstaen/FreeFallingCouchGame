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
	CreateAndInitsPlayers();
	ArenaActorInstance = GetWorld()->SpawnActor<AArenaActor>();
	//TODO Find way to receive player made modifications
	StartMatch();
}
#pragma region CharacterSpawn
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
	uint8 ID_Player = 1;
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
		NewCharacter->setIDPlayerLinked(ID_Player);
		/*NewCharacter->SetOrientX(SpawnPoint->GetStartOrientX());*/

		CharactersInsideArena.Add(NewCharacter);
		ID_Player++;
	}
}
#pragma endregion

#pragma region PreRound
void AFreeFallGameMode::StartMatch()
{
	// Check Player data
	if(!IsValid(PlayerMatchData))
		PlayerMatchData = NewObject<UPlayerMatchData>();
	PlayerMatchData->resetScoreValue();
	
	ArenaActorInstance->OnCharacterDestroyed.AddDynamic(this, &AFreeFallGameMode::CheckEndRound);
	SetupMatch(nullptr);
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("---------------------MATCH START--------------------"));
	StartRound();
}
void AFreeFallGameMode::StartRound()
{
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(TEXT("Current Round: %i"), CurrentRound));

	TArray<APlayerStart*> PlayerStartsPoints;
	FindPlayerStartActorsInMap(PlayerStartsPoints);
	SpawnCharacters(PlayerStartsPoints);
	ArenaActorInstance->Init(this);

	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("---------------------ROUND START--------------------"));
	if(OnStartRound.IsBound())
	{
		OnStartRound.Broadcast();
	}
	CurrentRound++;
	if(CurrentParameters->getTimerDelay() > 0.f)
	{
		RoundEventTimer();	
	}
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

#pragma region DuringRound

void AFreeFallGameMode::CheckEndRound(AFreeFallCharacter* Character)
{
	//TODO Array of order in which characters got eliminated
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple,
		FString::Printf(TEXT("Player number %i was eliminated!"), Character->getIDPlayerLinked()));
	LossOrder.insert(LossOrder.begin(), Character->getIDPlayerLinked());
	CharactersInsideArena.Remove(Character);
	if(CharactersInsideArena.Num() <= 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "One character, remaining, end match!");
		AddPoints();
		EndRound();
	}
}
void AFreeFallGameMode::RoundEventTimer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartTimer");
	GetWorld()->GetTimerManager().SetTimer(
		RoundTimerHandle,
		this,
		&AFreeFallGameMode::StartEvent,
		CurrentParameters->getTimerDelay(),
		true
		);
}

void AFreeFallGameMode::StartEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartEvent");
	if(OnCallEvent.IsBound()) OnCallEvent.Broadcast();
	//Here can be implemented a random function to start random events
}
#pragma endregion

#pragma region PostRound

void AFreeFallGameMode::AddPoints()
{
	std::vector<uint8> RoundRanking;
	if(CharactersInsideArena.Num() == 1)
	{
		RoundRanking.push_back(CharactersInsideArena[0]->getIDPlayerLinked());
	}
	// Append lossOrder to RoundRanking
	RoundRanking.insert(RoundRanking.end(), LossOrder.begin(), LossOrder.end());
	if(IsValid(PlayerMatchData))
	{
		// Assign points
		const int*temp = 	CurrentParameters->getScoreValues();
		for(int x = 0; x < RoundRanking.size(); x++)
		{
			PlayerMatchData->setScoreValue(RoundRanking[x], temp[x]);
		}
	}
	// Debug, can and should be removed
	int i = 1;
	for(uint8 Ranking : RoundRanking)
	{
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Yellow,
FString::Printf(TEXT("%i spot is taken by player id: %i !"), i, Ranking));
		i++;
	}
	// Empty lossOrder
	LossOrder.clear();
}

void AFreeFallGameMode::EndRound()
{
	// Clear Timer
	if(GetWorldTimerManager().IsTimerActive(RoundTimerHandle))
		GetWorldTimerManager().ClearTimer(RoundTimerHandle);
	
	// Reset CharactersInside Arena
	for (auto Element : CharactersInsideArena) { Element->Destroy();}
	CharactersInsideArena.Empty();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "EndRound");
	
	// Unlink event (to reapply properly later on, avoiding double linkage)
	if(OnEndRound.IsBound())
		OnEndRound.Broadcast();
	// Check for end match
	if(CurrentRound >= CurrentParameters->getMaxRounds())
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("---------------------MATCH END--------------------"));
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
	ArenaActorInstance->OnCharacterDestroyed.RemoveDynamic(this, &AFreeFallGameMode::CheckEndRound);

	//TODO Remove Debug
	PlayerMatchData->DebugPrintScore();
	
	if(OnResults.IsBound())
	{
		OnResults.Broadcast();
	}
	//TODO AwaitUserInput
	StartMatch();
}
#pragma endregion