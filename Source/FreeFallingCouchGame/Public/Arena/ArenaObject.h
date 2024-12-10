// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Characters/FreeFallCharacter.h"
#include "GameFramework/Actor.h"
#include "Other/Parachute.h"
#include "ArenaObject.generated.h"

class UCharacterIndicatorWidget;
class AFreeFallGameMode;

//TODO Transform Arena Actor to tickable Object

UCLASS()
class FREEFALLINGCOUCHGAME_API UArenaObject : public UObject, public FTickableGameObject
{
	GENERATED_BODY()

#pragma region Init

public:
	// Sets default values for this actor's properties
	UArenaObject();
	UFUNCTION()
	void Init(const AFreeFallGameMode* FreeFallGameMode);

	UCameraComponent* FindCameraByTag(const FName& Tag) const;

#pragma endregion

#pragma region Properties

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
	TArray<TObjectPtr<AFreeFallCharacter>> CharactersInsideArena;

	UPROPERTY()
	TObjectPtr<AParachute> Parachute;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain;

#pragma endregion 
	
#pragma region Check Out Of Bounds
	
public :
	UFUNCTION()
	void CheckAndRemoveOutOfBoundPlayers();

	UE_DEPRECATED(5.3, "Function is depreciated, no need to CheckOutOfBoundParachute anymore")
	UFUNCTION()
	void CheckOutOfBoundParachute();

	bool IsOutOfBounds(FVector2D ScreenPosition, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax) const;

	bool IsNearOutOfBounds(FVector2D ScreenPosition, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax) const;
	
	void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDestroyed, AFreeFallCharacter*, Character);
	UPROPERTY(BlueprintAssignable)
	FOnCharacterDestroyed OnCharacterDestroyed;
	
#pragma endregion 
	
#pragma region Tickable Properties

public:
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FMyTickableThing, STATGROUP_Tickables); }
	virtual bool IsTickableWhenPaused() const override { return false; }
	virtual bool IsTickableInEditor() const override { return false; } 
	
private:
	//To check if we call it multiple frames
	uint32 LastFrameNumberWeTicked = INDEX_NONE;

#pragma endregion

#pragma region UI

protected:
	UPROPERTY()
	TArray<TObjectPtr<UCharacterIndicatorWidget>> CharacterIndicatorWidgets;

	void CreateCharacterIndicatorWidget(TSubclassOf<UUserWidget> CharacterIndicatorWidgetClass, int PlayerIndex);

	void DisplayOffScreenCharacterIndicators();

	TObjectPtr<UCharacterIndicatorWidget> GetCharacterIndicatorWidgetFromPlayerIndex(int PlayerIndex);

	//Check if position is outside of Viewport without tolerance, return true if Position is outside of Viewport;
	bool IsOutsideOfViewport(FVector2D ScreenPos, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax);

#pragma endregion
	
};
