// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacter.h"
#include "GameFramework/Actor.h"
#include "ArenaActor.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AArenaActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AArenaActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void Init();
	
	UPROPERTY(EditAnywhere)
	float OffScreenTolerance;
	
	UPROPERTY()
	TArray<AFreeFallCharacter*> CharactersInsideArena;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDestroyed, AFreeFallCharacter*, Character);
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDestroyed OnCharacterDestroyed;
};
