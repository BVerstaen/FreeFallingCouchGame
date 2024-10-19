// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

protected:
	UPROPERTY(EditAnywhere)
	TArray<AFreeFallCharacter*> CharactersInsideArena;

private:
	void CreateAndInitsPlayers() const;
	
	void FindPlayerStartActorsInMap(TArray<APlayerStart*>& ResultsActors);

	TSubclassOf<AFreeFallCharacter> GetFreeFallCharacterClassFromInputType(EAutoReceiveInput::Type InputType) const;

	void SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints);
	
};
