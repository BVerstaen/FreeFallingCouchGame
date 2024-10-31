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
	TrackerActorInstance = GetWorld()->SpawnActor<ATrackerActor>();

	//Find Parachute Spawnlocation then destroy dummy parachute
	AParachute* Parachute = Cast<AParachute>(UGameplayStatics::GetActorOfClass(GetWorld(), AParachute::StaticClass()));
	if(Parachute)
	{
		ParachuteSpawnLocation = Parachute->GetActorLocation();
		Parachute->Destroy();		
	}
	else
	{
		ParachuteSpawnLocation = FVector(0, 0, 0);
	}
	
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

AParachute* AFreeFallGameMode::RespawnParachute(FVector SpawnLocation)
{
	//Destroy parachute if already exists
	if(ParachuteInstance)
		ParachuteInstance->Destroy();
	
	//Get map settings & set location
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	FTransform SpawnTransform = FTransform::Identity;
	SpawnTransform.SetLocation(SpawnLocation);

	//Spawn parachute
	return GetWorld()->SpawnActorDeferred<AParachute>(MapSettings->ParachuteSubclass, SpawnTransform);
}

AParachute* AFreeFallGameMode::GetParachuteInstance() const
{
	return ParachuteInstance;
}

#pragma endregion

#pragma region PreRound
void AFreeFallGameMode::StartMatch()
{
	// Check Player data
	if(!IsValid(PlayerMatchData))
		PlayerMatchData = NewObject<UPlayerMatchData>();
	PlayerMatchData->resetScoreValue();
	
	ArenaActorInstance->OnCharacterDestroyed.AddDynamic(this, &AFreeFallGameMode::CheckEndRoundDeath);
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
	ParachuteInstance = RespawnParachute(ParachuteSpawnLocation);
	ArenaActorInstance->Init(this);
	TrackerActorInstance->Init(ParachuteInstance, CharactersInsideArena);
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("---------------------ROUND START--------------------"));
	CurrentRound++;
	if(CurrentParameters->getTimerEventDelay() > 0.f)
		RoundEventTimer();
	if(CurrentParameters->getRoundTimer() > 0.f)
		RoundTimer();
	if(OnStartRound.IsBound())
	{
		OnStartRound.Broadcast();
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

void AFreeFallGameMode::StartEvent()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartEvent");
	if(OnCallEvent.IsBound()) OnCallEvent.Broadcast();
}

void AFreeFallGameMode::CheckEndRoundTimer()
{
	ClearTimers();

	if(IsValid(TrackerActorInstance))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
			"Timer end, end match!");
		ArenaActorInstance->OnCharacterDestroyed.RemoveDynamic(this, &AFreeFallGameMode::CheckEndRoundDeath);
		AddPoints(TrackerActorInstance->GetTrackingWinners(CurrentParameters->getTrackingRewardCategory()));
		EndRound();
	}
}

void AFreeFallGameMode::CheckEndRoundDeath(AFreeFallCharacter* Character)
{
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple,
		FString::Printf(TEXT("Player number %i was eliminated!"), Character->getIDPlayerLinked()));
	LossOrder.Insert(Character->getIDPlayerLinked(), 0);
	CharactersInsideArena.Remove(Character);
	if(CharactersInsideArena.Num() <= 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "One character, remaining, end match!");
		ArenaActorInstance->OnCharacterDestroyed.RemoveDynamic(this, &AFreeFallGameMode::CheckEndRoundDeath);
		TrackerActorInstance->RemoveDelegates();
		AddPoints(SetDeathOrder());
		EndRound();
	}
}
TArray<int> AFreeFallGameMode::SetDeathOrder()
{
	TArray<int> RoundRanking;
	if(CharactersInsideArena.Num() == 1)
		RoundRanking.Add(CharactersInsideArena[0]->getIDPlayerLinked());
	// Append lossOrder to RoundRanking
	RoundRanking.Append(LossOrder);
	return RoundRanking;
}
#pragma endregion

#pragma region PostRound

void AFreeFallGameMode::AddPoints(TArray<int> ArrayPlayers)
{
	if(IsValid(PlayerMatchData))
	{
		// Assign points
		const int*temp = CurrentParameters->getScoreValues();
		for (int i  = 0; i< ArrayPlayers.Num(); i++)
			PlayerMatchData->setScoreValue(ArrayPlayers[i], temp[i]);
	}
	// Empty lossOrder
	LossOrder.Empty();
}


void AFreeFallGameMode::EndRound()
{
	ClearTimers();
	// Reset CharactersInside Arena
	
	for (AFreeFallCharacter* Element : CharactersInsideArena) { Element->Destroy();}
	
	CharactersInsideArena.Empty();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "EndRound");

	//Give extra points for rewards
	TArray<int> ExtraPoints = TrackerActorInstance->GiveWinners();
	for(int ExtraPointWinner : ExtraPoints)
	{
		PlayerMatchData->setScoreValue(ExtraPointWinner, 1);
	}
	//Just a debug message to make sure the tracker works, meant to be removed
	TrackerActorInstance->DebugPrintResultReward();
	
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
//-------------------------------------------TIMERS---------------------------------------------------------
#pragma region Timers

void AFreeFallGameMode::ClearTimers()
{
	if(GetWorldTimerManager().IsTimerActive(EventTimerHandle))
		GetWorldTimerManager().ClearTimer(EventTimerHandle);
	if(GetWorldTimerManager().IsTimerActive(RoundTimerHandle))
		GetWorldTimerManager().ClearTimer(RoundTimerHandle);
}

void AFreeFallGameMode::RoundEventTimer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartTimer");
	GetWorld()->GetTimerManager().SetTimer(
		EventTimerHandle,
		this,
		&AFreeFallGameMode::StartEvent,
		CurrentParameters->getTimerEventDelay(),
		true
		);
}

void AFreeFallGameMode::RoundTimer()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "StartTimer");
	GetWorld()->GetTimerManager().SetTimer(
		RoundTimerHandle,
		this,
		&AFreeFallGameMode::CheckEndRoundTimer,
		CurrentParameters->getRoundTimer(),
		false
		);
}
#pragma endregion