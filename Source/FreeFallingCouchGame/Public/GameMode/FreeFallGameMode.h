// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "FreeFallingCouchGame/Public/Match/MatchParameters.h"
#include "Characters/FreeFallCharacterInputData.h"
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
	
	UPROPERTY(EditAnywhere)
	TArray<AFreeFallCharacter*> CharactersInsideArena;

private:
	void CreateAndInitsPlayers() const;

	UFreeFallCharacterInputData* LoadInputDataFromConfig();

	UInputMappingContext* LoadInputMappingContextFromConfig();
	
	void FindPlayerStartActorsInMap(TArray<APlayerStart*>& ResultsActors);

	TSubclassOf<AFreeFallCharacter> GetFreeFallCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const;

	void SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints);

#pragma region Rounds
protected:
	UPROPERTY(EditAnywhere)
	uint8 CurrentRound = 0;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMatchParameters> DefaultParameters = nullptr;
	UPROPERTY()
	UMatchParameters *CurrentParameters;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDStartRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDEndRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDResults);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDCallEvent);
	FDStartRound OnStartRound;
	FDEndRound OnEndRound;
	FDResults OnResults;
	FDCallEvent OnCallEvent;
	FTimerHandle RoundTimerHandle;
	
private:
	void StartRound();
	void RoundEventTimer();
	void EndRound();
	void ShowResults();
	void StartEvent();
	void SetupMatch(TSubclassOf<UMatchParameters> UserParameters);
#pragma endregion
};