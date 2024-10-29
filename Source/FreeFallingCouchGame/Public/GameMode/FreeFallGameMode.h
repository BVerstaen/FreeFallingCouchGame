// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Arena/ArenaActor.h"
#include "Arena/TrackerActor.h"
#include "FreeFallingCouchGame/Public/Match/MatchParameters.h"
#include "Characters/PlayerMatchData.h"
#include "GameFramework/GameModeBase.h"
#include "FreeFallGameMode.generated.h"


class AFreeFallCharacter;

class APlayerStart;

UCLASS()
class FREEFALLINGCOUCHGAME_API AFreeFallGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	void StartMatch();

	UPROPERTY(EditAnywhere)
	TArray<AFreeFallCharacter*> CharactersInsideArena;

	UFUNCTION()
	AParachute* GetParachuteInstance() const;
	
private:
	void CreateAndInitsPlayers() const;

	UFreeFallCharacterInputData* LoadInputDataFromConfig();

	UInputMappingContext* LoadInputMappingContextFromConfig();
	
	void FindPlayerStartActorsInMap(TArray<APlayerStart*>& ResultsActors);

	TSubclassOf<AFreeFallCharacter> GetFreeFallCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const;

	void SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints);

	AParachute* RespawnParachute();
	
#pragma region Rounds
protected:
	
	
	UPROPERTY(EditAnywhere)
	uint8 CurrentRound = 0;
	FTimerHandle RoundTimerHandle;
	FTimerHandle EventTimerHandle;
	//Ranking
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerMatchData> PlayerMatchData;
	std::vector<uint8> LossOrder;
	// Refs to Objects in Scene
	UPROPERTY()
	TObjectPtr<AArenaActor> ArenaActorInstance;

	UPROPERTY()
	TObjectPtr<ATrackerActor> TrackerActorInstance;

	UPROPERTY()
	TObjectPtr<AParachute> ParachuteInstance;
	// Match Parameters
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMatchParameters> DefaultParameters = nullptr;

	UPROPERTY()
	UMatchParameters *CurrentParameters;
	// Delegate declaration
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDStartRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDEndRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDResults);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDCallEvent);
	// Delegate Instance
	FDStartRound OnStartRound;
	FDEndRound OnEndRound;
	FDResults OnResults;
	FDCallEvent OnCallEvent;
private:
	//After Initiation, launches the timer and links events
	void StartRound();
	// Timers
	void RoundEventTimer();
	void RoundTimer();
	void ClearTimers();
	// When Round end condition is reached, unlinks and check if match is over
	void EndRound();
	void CheckEndRoundTimer();
	// When Match is over, calls an event to show
	// Results UI and buttons to go back to menu (/ restart)
	void ShowResults();
	// What RoundEventTimer calls 
	void StartEvent();
	void AddPoints();
	UFUNCTION()
	// Checks if end condition is reached
	void CheckEndRoundDeath(AFreeFallCharacter* Character);
	// Sets up the values for the match & rounds to follow
	void SetupMatch(TSubclassOf<UMatchParameters> UserParameters);
#pragma endregion
};
