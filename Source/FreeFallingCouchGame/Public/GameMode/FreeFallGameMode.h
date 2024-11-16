// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Arena/ArenaObject.h"
#include "Arena/TrackerActor.h"
#include "FreeFallingCouchGame/Public/Match/MatchParameters.h"
#include "Characters/PlayerMatchData.h"
#include "GameFramework/GameModeBase.h"
#include "UI/Widgets/RoundCounterWidget.h"
#include "UI/Widgets/RoundScorePanelWidget.h"
#include "FreeFallGameMode.generated.h"


class ULevelStreamingDynamic;
class AFreeFallCharacter;

class APlayerStart;

UCLASS()
class FREEFALLINGCOUCHGAME_API AFreeFallGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	void StartMatch();
	void Init();
	
	UPROPERTY(EditAnywhere)
	TArray<AFreeFallCharacter*> CharactersInsideArena;
	
	UFUNCTION()
	AParachute* GetParachuteInstance() const;

#pragma region Player Start Level Instanciation
private:
	UFUNCTION()
	void OnSubLevelPlayerStartLoaded();
	void VerifyLevelVisibility();
	FTimerHandle SubLevelTimerHandle;
#pragma endregion 
	
private:
	void CreatePlayerStarts();
	
	void CreateAndInitsPlayers() const;

	UFreeFallCharacterInputData* LoadInputDataFromConfig();

	UInputMappingContext* LoadInputMappingContextFromConfig();
	
	void FindPlayerStartActorsInMap(TArray<APlayerStart*>& ResultsActors);

	TSubclassOf<AFreeFallCharacter> GetFreeFallCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const;

	void SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints);

	bool GetCharacterInvertDiveInput(int PlayerIndex);
	
	AParachute* RespawnParachute(FVector SpawnLocation);

	FVector ParachuteSpawnLocation;

#pragma region Widgets
	//Match counter widget
	UPROPERTY()
	TObjectPtr<URoundCounterWidget> RoundCounterWidget;

	UPROPERTY()
	TObjectPtr<URoundScorePanelWidget> RoundScorePanelWidget;
#pragma endregion
	
#pragma region Rounds
protected:


	
	UPROPERTY(EditAnywhere)
	float CurrentCounter;
	
	
	UPROPERTY(EditAnywhere)
	uint8 CurrentRound = 0;
	FTimerHandle RoundTimerHandle;
	FTimerHandle EventTimerHandle;
	
	//Ranking
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerMatchData> PlayerMatchData;
	
	//std::vector<uint8> LossOrder;
	TArray<int> LossOrder;
	
	// Refs to Objects in Scene
	UPROPERTY()
	TObjectPtr<UArenaObject> ArenaActorInstance;

	UPROPERTY()
	TObjectPtr<ATrackerActor> TrackerActorInstance;

	UPROPERTY()
	TObjectPtr<AParachute> ParachuteInstance;
	
	// Match Parameters
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMatchParameters> DefaultParameters = nullptr;

	UPROPERTY(BlueprintReadOnly)
	UMatchParameters *CurrentParameters;

public:
	// Delegate declaration
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDStartRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDEndRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDResults);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDCallEvent);
	// Delegate Instance
	UPROPERTY(BlueprintAssignable, Category = "EventsFreefall")
	FDStartRound OnStartRound;
	UPROPERTY(BlueprintAssignable, Category = "EventsFreefall")
	FDEndRound OnEndRound;
	UPROPERTY(BlueprintAssignable, Category = "EventsFreefall")
	FDResults OnResults;
	UPROPERTY(BlueprintAssignable, Category = "EventsFreefall")
	FDCallEvent OnCallEvent;
	
public:
	UFUNCTION(BlueprintCallable)
	FTimerHandle getRoundTimer() const { return RoundTimerHandle; }
	
	//After Initiation, launches the timer and links events
	UFUNCTION()
	void StartRound();
	
private:
	// Timers
	void RoundEventTimer();
	void RoundTimer();
	void ClearTimers();

	/*
	 *	End round functions
	 */
	UPROPERTY()
	TArray<int> OldPlayerScore;
	// When Round end condition is reached, unlinks and check if match is over
	void EndRound();
	//Timer Handle for timers
	FTimerHandle EndRoundTimerHandle;
	// Launch add score after round ended
	UFUNCTION()
	void EndRoundAddScore();
	// Launch add score after round ended
	UFUNCTION()
	void EndRoundCycleAddRewardPoints();
	int CurrentCategory;
	
	UFUNCTION()
	void EndRoundWaitHide();
	UFUNCTION()
	void EndRoundHideScorePanel();
	UFUNCTION()
	bool EndRoundAddRewardPoints(ETrackingRewardCategory Category, float DelayOnScreen);
	
	void CheckEndRoundTimer();
	// When Match is over, calls an event to show
	// Results UI and buttons to go back to menu (/ restart)
	void ShowResults();
	// What RoundEventTimer calls 
	void StartEvent();
	// Adding points to players
	TArray<int> SetDeathOrder();
	void AddPoints(TArray<int> ArrayPlayers);
	UFUNCTION()
	// Checks if end condition is reached
	void CheckEndRoundDeath(AFreeFallCharacter* Character);
	// Sets up the values for the match & rounds to follow
	//void SetupMatch(TSubclassOf<UMatchParameters> UserParameters);
	void SetupMatch(UMatchParameters *UserParameters);
	
#pragma endregion
};
