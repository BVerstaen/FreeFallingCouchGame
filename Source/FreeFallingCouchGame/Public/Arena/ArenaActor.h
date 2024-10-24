// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacter.h"
#include "GameFramework/Actor.h"
#include "ArenaActor.generated.h"

class AFreeFallGameMode;

UCLASS()
class FREEFALLINGCOUCHGAME_API AArenaActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArenaActor();

protected:
	UPROPERTY()
	float OffScreenTolerance;
	
	UPROPERTY()
	float NearEdgeScreenTolerance;
	
	UPROPERTY()
	TArray<AFreeFallCharacter*> CharactersInsideArena;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void Init(const AFreeFallGameMode* FreeFallGameMode);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDestroyed, AFreeFallCharacter*, Character);
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDestroyed OnCharacterDestroyed;
};
