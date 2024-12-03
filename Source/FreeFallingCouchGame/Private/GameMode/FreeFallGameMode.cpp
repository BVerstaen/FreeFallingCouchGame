// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/FreeFallGameMode.h"

#include "LocalMultiplayerSubsystem.h"
#include "Audio/SoundSubsystem.h"
#include "Characters/FreeFallCharacter.h"
#include "Engine/LevelStreamingDynamic.h"
#include "GameFramework/PlayerStart.h"
#include "Haptic/HapticsSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Match/GameDataInstanceSubsystem.h"
#include "Settings/CharactersSettings.h"
#include "Settings/MapSettings.h"

void AFreeFallGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	if(LocalMultiplayerSubsystem == nullptr) return;
	NumberOfPlayers = MapSettings->bActivateControlsInGame? MapSettings->NumberOfPlayers : LocalMultiplayerSubsystem->NumberOfPlayers;
	
	CreatePlayerStarts();
}

void AFreeFallGameMode::Init()
{
	CreateAndInitsPlayers();
	ArenaActorInstance = NewObject<UArenaObject>(GetWorld());
	TrackerActorInstance = GetWorld()->SpawnActor<ATrackerActor>();
	GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataInstanceSubsystem>();
	

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
	
	//Add wobble to camera
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->ClientStartCameraShake(GetDefault<UMapSettings>()->CameraShake, 50);

	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	LocalMultiplayerSubsystem->bCanCreateNewPlayer = MapSettings->bActivateControlsInGame;

	// receive match data from GameDataInstanceSubsystem and check its validity to start the match, Setup CurrentParameters Object
	GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataInstanceSubsystem>();
	if(GameDataSubsystem->IsValidLowLevel())
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, TEXT("Subsystem valid, checking match data"));
		UMatchParameters* refParameters = GameDataSubsystem->GetMatchParameters();
		if(refParameters->IsValidLowLevel())
		{
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Blue, TEXT("Data received valid, starting with Subsystem parameters"));
			SetupParameters(refParameters);
		} else {
			GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("Data not received, starting with default parameters"));
			SetupParameters(nullptr);
		}
	} else {
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("Subsystem invalid, starting with default parameters"));
		SetupParameters(nullptr);
	}
	
	//TODO Find way to receive player made modifications

	if (GameDataSubsystem->CurrentRound == 0)
	{
		StartMatch();
	}
	else
	{
		OldPlayerScore.Empty();
		for (int i = 0; i < NumberOfPlayers; i++)
		{
			OldPlayerScore.Add(GameDataSubsystem->GetPlayerScoreFromID(i));
		}
		StartRound();
	}
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

void AFreeFallGameMode::CreatePlayerStarts()
{
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	if (MapSettings == nullptr) return;
	
	const TArray<TSoftObjectPtr<UWorld>>& WorldList = MapSettings->PlayerStartsLevels;
	FVector SpawnLocation = MapSettings->PlayerStartSubLevelLocation;
	
	if(NumberOfPlayers <= 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Number of player is below 2, NumberOfPlayers need at least 2");
		return;
	}
	TSoftObjectPtr<UWorld> PlayerStartLevelToLoad = WorldList[NumberOfPlayers - 2];
	
	bool bLoadSuccessful = false;
	ULevelStreamingDynamic* StreamingLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(),
		PlayerStartLevelToLoad, SpawnLocation, FRotator::ZeroRotator, bLoadSuccessful);
	
	if(!bLoadSuccessful)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Load not successful");
		return;
	}

	StreamingLevel->OnLevelLoaded.AddDynamic(this, &AFreeFallGameMode::OnSubLevelPlayerStartLoaded);
}

UFreeFallCharacterInputData* AFreeFallGameMode::LoadInputDataFromConfig()
{
	const UCharactersSettings* CharacterSettings = GetDefault<UCharactersSettings>();
	if (CharacterSettings == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("CharactersSettings est nullptr"));
		return nullptr;
	}
	GetGameInstance()->GetSubsystem<UHapticsSubsystem>()->InitHaptics(CharacterSettings->RumbleSystem);
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
	for (ULevelStreaming* StreamingLevel :  GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel && StreamingLevel->IsLevelLoaded())
		{
			ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel();
			if (LoadedLevel)
			{
				for (AActor* Actor : LoadedLevel->Actors)
				{
					if (APlayerStart* PlayerStartActor = Cast<APlayerStart>(Actor))
					{
						ResultsActors.Add(PlayerStartActor);
					}
				}
			}
		}
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

bool AFreeFallGameMode::GetCharacterInvertDiveInput(int PlayerIndex)
{
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	if (MapSettings == nullptr) return false;

	switch (PlayerIndex - 1)
	{
	case 0:
		return MapSettings->CharacterP0InvertDiveControl;
	case 1:
		return MapSettings->CharacterP1InvertDiveControl;
	case 2:
		return MapSettings->CharacterP2InvertDiveControl;
	case 3:
		return MapSettings->CharacterP3InvertDiveControl;
	default:
		return false;
	}
}

void AFreeFallGameMode::SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints, bool bLockControls)
{
	UFreeFallCharacterInputData* InputData = LoadInputDataFromConfig();
	UInputMappingContext* InputMappingContext = LoadInputMappingContextFromConfig();
	uint8 ID_Player = 0;
	
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
		NewCharacter->InvertDiveInput = GetCharacterInvertDiveInput(ID_Player);
		NewCharacter->setIDPlayerLinked(ID_Player);
		NewCharacter->SetLockControls(bLockControls);
		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());
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
	AParachute* NewParachuteInstance = GetWorld()->SpawnActorDeferred<AParachute>(MapSettings->ParachuteSubclass, SpawnTransform);
	FTransform ParachuteTransform = FTransform::Identity;
	ParachuteTransform.SetLocation(ParachuteSpawnLocation);
	NewParachuteInstance->FinishSpawning(ParachuteTransform);
	
	return NewParachuteInstance;
}

void AFreeFallGameMode::CallArenaActorOnCharacterDestroyed(AFreeFallCharacter* Character)
{
	ArenaActorInstance->OnCharacterDestroyed.Broadcast(Character);
}

AParachute* AFreeFallGameMode::GetParachuteInstance() const
{
	return ParachuteInstance;
}

/*
 *	Idea -> wait until player start sublevel is loaded before continuing Initialisation
 */
void AFreeFallGameMode::OnSubLevelPlayerStartLoaded() { GetWorld()->GetTimerManager().SetTimer(SubLevelTimerHandle, this, &AFreeFallGameMode::VerifyLevelVisibility, 0.1f, true); }
void AFreeFallGameMode::VerifyLevelVisibility()
{
	bool bAllLevelsVisible = true;

	//Check if every streaming level is Visible
	for (ULevelStreaming* StreamingLevel : GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel && !StreamingLevel->IsLevelVisible())
		{
			bAllLevelsVisible = false;
			break;
		}
	}
	//If so -> stop timer and continue initialisation
	if (bAllLevelsVisible)
	{
		GetWorld()->GetTimerManager().ClearTimer(SubLevelTimerHandle);
		Init();
	}
}

#pragma endregion

#pragma region PreRound
void AFreeFallGameMode::StartMatch()
{
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	if(!MapSettings) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("---------------------MATCH START--------------------"));

	//Reset next parachute holder ID
	GameDataSubsystem->NextParachuteHolderID = -1;
	
	//Spawn Characters for pre match
	TArray<APlayerStart*> PlayerStartsPoints;
	FindPlayerStartActorsInMap(PlayerStartsPoints);
	SpawnCharacters(PlayerStartsPoints, true);

	//Play gameplay music
	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlayMusic("MUS_GameplayTimer_ST");
	
	//Create parachute & equip to next player
	ParachuteInstance = RespawnParachute(ParachuteSpawnLocation);
	ParachuteInstance->OnParachuteGrabbed.AddDynamic(this, &AFreeFallGameMode::BeginFirstRound);

	FVector ParachuteBeginPosition = ParachuteInstance->GetActorLocation();
	ParachuteBeginPosition.Z += 1000;
	ParachuteInstance->SetActorLocation(ParachuteBeginPosition);
}

void AFreeFallGameMode::BeginFirstRound(AFreeFallCharacter* NewOwner)
{
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	if(!MapSettings) return;

	//Get first parachute holder
	GameDataSubsystem->NextParachuteHolderID = NewOwner->getIDPlayerLinked();
	
	//Reset Player score
	GameDataSubsystem->ResetPlayerScore();
	OldPlayerScore.Empty();
	for (int i = 0; i < NumberOfPlayers; ++i)
	{
		GameDataSubsystem->SetPlayerScoreFromID(i, 0);
		OldPlayerScore.Add(0);
	}
	
	//Round counter and delegate 
	RoundCounterWidget = CreateWidget<URoundCounterWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), MapSettings->RoundCounterWidget);
	if(RoundCounterWidget)
	{
		RoundCounterWidget->AddToViewport();
		RoundCounterWidget->OnFinishCounter.AddDynamic(this, &AFreeFallGameMode::StartRound);
	}
}

void AFreeFallGameMode::PlayParachuteFallingAnimation()
{
	if(ParachuteInstance)
	{
		ParachuteInstance->PlayFallDownAnimation(ParachuteSpawnLocation);
	}
}

void AFreeFallGameMode::StartRound()
{
	if (!GameDataSubsystem)
	{
		GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataInstanceSubsystem>();
	}
	//Unbind Start round delegate
	if(RoundCounterWidget)
	{
		RoundCounterWidget->OnFinishCounter.RemoveDynamic(this, &AFreeFallGameMode::StartRound);
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(TEXT("Current Round: %i"), GameDataSubsystem->CurrentRound));

	//Spawn players if not already
	if(CharactersInsideArena.Num() <= 0)
	{
		TArray<APlayerStart*> PlayerStartsPoints;
		FindPlayerStartActorsInMap(PlayerStartsPoints);
		SpawnCharacters(PlayerStartsPoints);		
	}
	//Else release player controls
	else
	{
		for(AFreeFallCharacter* Character : CharactersInsideArena)
		{
			Character->SetLockControls(false);
		}
	}
	//Create parachute & equip to next player
	ParachuteInstance = RespawnParachute(ParachuteSpawnLocation);
	ParachuteInstance->OnParachuteDropped.AddDynamic(this, &AFreeFallGameMode::FindNewOwnerForParachute);
	for(AFreeFallCharacter* Character : CharactersInsideArena)
	{
		if(Character->getIDPlayerLinked() == GameDataSubsystem->NextParachuteHolderID)
		{
			ParachuteInstance->EquipToPlayer(Character);
			break;
		}
	}
	
	ArenaActorInstance->Init(this);
	ArenaActorInstance->OnCharacterDestroyed.AddDynamic(this, &AFreeFallGameMode::CheckEndRoundDeath);
	TrackerActorInstance->Init(ParachuteInstance, CharactersInsideArena);
	//SetupMatch(nullptr); //Possiblement à enlever, j'ai juste rerajouté pour pas tout péter :)
	
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("---------------------ROUND START--------------------"));
	GameDataSubsystem->CurrentRound++;
	
	if(CurrentParameters->getTimerEventDelay() > 0.f)
		RoundEventTimer();
	if(CurrentParameters->getRoundTimer() > 0.f)
		RoundTimer();
	if(OnStartRound.IsBound())
	{
		OnStartRound.Broadcast();
	}
	ShowInGameUI();
}
//void AFreeFallGameMode::SetupMatch(TSubclassOf<UMatchParameters> UserParameters)
void AFreeFallGameMode::SetupParameters(UMatchParameters *UserParameters)
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
		
		TArray<int> DeathOrder = SetDeathOrder();
		AddPoints(DeathOrder);

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

void AFreeFallGameMode::FindNewOwnerForParachute(AFreeFallCharacter* PreviousOwner)
{
	if(!ParachuteInstance) return;
	if(CharactersInsideArena.Num() <= 0) return;

	AFreeFallCharacter* NewOwner = CharactersInsideArena[FMath::RandRange(0, CharactersInsideArena.Num() - 1)];
	ParachuteInstance->EquipToPlayer(NewOwner);
}

#pragma endregion

#pragma region PostRound

void AFreeFallGameMode::AddPoints(TArray<int> ArrayPlayersID)
{
	if(IsValid(GameDataSubsystem))
	{
		// Assign points
		const TArray<int> WinPoints = CurrentParameters->getScoreValues();
		for (int i  = 0; i< ArrayPlayersID.Num(); i++)
		{
			if(ArrayPlayersID.Num() >= i && WinPoints.Num() >= i) break;
			GameDataSubsystem->AddPlayerScoreFromID(ArrayPlayersID[i], WinPoints[i]);
		}
	}
	// Empty lossOrder
	LossOrder.Empty();
}


void AFreeFallGameMode::EndRound()
{
	ClearTimers();
	
	// Reset CharactersInside Arena
	for (AFreeFallCharacter* Element : CharactersInsideArena)
	{
		if(Element)
			Element->Destroy();
	}
	CharactersInsideArena.Empty();
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "EndRound");

	//remove Parachute instance dynamic
	ParachuteInstance->OnParachuteDropped.RemoveDynamic(this, &AFreeFallGameMode::FindNewOwnerForParachute);
	
	//Give next parachute to last player
	int MinimumScoreID = 0;
	for(int i = 0; i < NumberOfPlayers; i++)
	{
		if(GameDataSubsystem->GetPlayerScoreFromID(i) < GameDataSubsystem->GetPlayerScoreFromID(MinimumScoreID) && GameDataSubsystem->GetPlayerScoreFromID(i)!=-1)
			MinimumScoreID = i;
	}
	GameDataSubsystem->NextParachuteHolderID = MinimumScoreID;

	//Destroy parachute if already exists
	if(ParachuteInstance)
		ParachuteInstance->Destroy();
	
	//Create widget
	//Round Score panel
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	RoundScorePanelWidget = CreateWidget<URoundScorePanelWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), MapSettings->RoundScorePanelWidget);
	if(RoundScorePanelWidget)
	{
		RoundScorePanelWidget->AddToViewport();
		RoundScorePanelWidget->OnFinishShow.AddDynamic(this, &AFreeFallGameMode::EndRoundAddScore);

		//Set player profile
		int MaxNumberOfPoints = 8 * CurrentParameters->getMaxRounds();
		for(int i = 0; i < NumberOfPlayers; i++)
		{
			RoundScorePanelWidget->SetPlayerProfile(i+1, OldPlayerScore[i], MaxNumberOfPoints);
		}
	}
}

void AFreeFallGameMode::EndRoundAddScore()
{
	RoundScorePanelWidget->OnFinishShow.RemoveDynamic(this, &AFreeFallGameMode::EndRoundAddScore);
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();

	for(int i = 0; i < NumberOfPlayers; i++)
	{
		int NewScore = GameDataSubsystem->GetPlayerScoreFromID(i);
		RoundScorePanelWidget->AddScoreToRound(i + 1, NewScore);
		OldPlayerScore[i] = NewScore;
	}
	CurrentCategory = 0;
	GetWorld()->GetTimerManager().SetTimer(EndRoundTimerHandle, this, &AFreeFallGameMode::EndRoundCycleAddRewardPoints, MapSettings->TimeBeforeRewardPoints, false, MapSettings->TimeBeforeRewardPoints);
}

void AFreeFallGameMode::EndRoundCycleAddRewardPoints()
{
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	
	ETrackingRewardCategory Category = TrackerActorInstance->CategoriesOfAward[CurrentCategory];
	bool HasChangedReward = EndRoundAddRewardPoints(Category, MapSettings->TimeBetweenGivingRewards - .1f);
	
	CurrentCategory++;
	if(CurrentCategory >= TrackerActorInstance->CategoriesOfAward.Num())
	{
		EndRoundTimerHandle.Invalidate();
		GetWorld()->GetTimerManager().SetTimer(EndRoundTimerHandle, this, &AFreeFallGameMode::EndRoundWaitHide, MapSettings->TimeBeforeHideScorePanelWidget, false, MapSettings->TimeBeforeHideScorePanelWidget);
	}
	else if(HasChangedReward)
	{
		GetWorld()->GetTimerManager().SetTimer(EndRoundTimerHandle, this, &AFreeFallGameMode::EndRoundCycleAddRewardPoints, MapSettings->TimeBetweenGivingRewards, false, MapSettings->TimeBetweenGivingRewards);
	}
	else
	{
		//If no winner -> then shorter timer
		GetWorld()->GetTimerManager().SetTimer(EndRoundTimerHandle, this, &AFreeFallGameMode::EndRoundCycleAddRewardPoints, 0.1f, false, 0.1f);
	}
}

void AFreeFallGameMode::EndRoundWaitHide()
{
	RoundScorePanelWidget->HideRoundScoreAnimation();
	RoundScorePanelWidget->OnFinishHide.AddDynamic(this, &AFreeFallGameMode::EndRoundHideScorePanel);
}

void AFreeFallGameMode::EndRoundHideScorePanel()
{
	RoundScorePanelWidget->OnFinishHide.RemoveDynamic(this, &AFreeFallGameMode::EndRoundHideScorePanel);
	RoundScorePanelWidget->RemoveFromParent();

	// Unlink event (to reapply properly later on, avoiding double linkage)
	if(OnEndRound.IsBound())
		OnEndRound.Broadcast();
	
	// Check for end match
	if(GameDataSubsystem->CurrentRound >= CurrentParameters->getMaxRounds())
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Red, TEXT("---------------------MATCH END--------------------"));
		ShowResults();
	} else
	{
		//StartRound();
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Silver, FString::FromInt(GameDataSubsystem->CurrentRound));
		//ZIZICACA
	}
}

bool AFreeFallGameMode::EndRoundAddRewardPoints(ETrackingRewardCategory Category, float DelayOnScreen)
{
	bool DidChangeAnyScore = false;
	
	//Set points for category
	TArray<int> ExtraPoints = TrackerActorInstance->GetTrackingWinners(Category);
	for(int ExtraPointWinner : ExtraPoints)
	{
		GameDataSubsystem->AddPlayerScoreFromID(ExtraPointWinner, 1);
	}
	
	//Display new score if gained points
	
	for(int i = 0; i < NumberOfPlayers; i++){
		int NewScore = GameDataSubsystem->GetPlayerScoreFromID(i);
		if (OldPlayerScore[i] != NewScore)
		{
			RoundScorePanelWidget->AddScoreReward(i+1, NewScore, Category, DelayOnScreen);
			OldPlayerScore[i] = NewScore;
			DidChangeAnyScore = true;
		}
	}

	return DidChangeAnyScore;
}

void AFreeFallGameMode::ShowResults()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, "ShowResults");
	GameDataSubsystem->CurrentRound = 0;
	
	if(OnResults.IsBound())
	{
		OnResults.Broadcast();
	}
	
	const UMapSettings* MapSettings = GetDefault<UMapSettings>();
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetWorld(), MapSettings->VictoryScreenLevel);
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