// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacter.h"
#include "GameFramework/Actor.h"
#include "Other/Parachute.h"
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
	float OffScreenHorizontalTolerance;

	UPROPERTY()
	float OffScreenVerticalTolerance;
	
	UPROPERTY()
	float NearEdgeScreenTolerance;
	
	UPROPERTY()
	TObjectPtr<APlayerController> MainCameraController;
	
	UPROPERTY()
	TArray<AFreeFallCharacter*> CharactersInsideArena;

	UPROPERTY()
	TObjectPtr<AParachute> Parachute;

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void Init(const AFreeFallGameMode* FreeFallGameMode);

	UFUNCTION()
	void CheckAndRemoveOutOfBoundPlayers();

	UE_DEPRECATED(5.3, "Function is depreciated, no need to CheckOutOfBoundParachute anymore")
	UFUNCTION()
	void CheckOutOfBoundParachute();

	bool IsOutOfBounds(FVector2D ScreenPosition, FVector2D ViewportSize) const;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDestroyed, AFreeFallCharacter*, Character);
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDestroyed OnCharacterDestroyed;

	
};
