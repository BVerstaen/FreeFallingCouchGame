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
	UMatchParameters DefaultParameters;
	UMatchParameters CurrentParameters;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDStartRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDEndRound);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDResults);
	UPROPERTY(BlueprintCallable)
	FDStartRound OnStartRound;
	UPROPERTY(BlueprintCallable)
	FDEndRound OnEndRound;
	UPROPERTY(BlueprintCallable)
	FDResults OnResults;

	FTimerHandle RoundTimerHandle;
	
private:
	void StartRound();
	void RoundEventTimer();
	void EndRound();
	void ShowResults();
	void StartEvent();
	void SetupMatch(const UMatchParameters& UserParameters);
#pragma endregion
};