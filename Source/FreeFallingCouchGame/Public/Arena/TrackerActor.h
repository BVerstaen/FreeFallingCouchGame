// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Other/Parachute.h"
#include "UObject/Object.h"
#include "TrackerActor.generated.h"

/**
 * 
 */
class AFreeFallGameMode;

UENUM()
enum ETrackingRewardCategory
{
	MostStealParachute,
	LongestTimeWithParachute,
	MostElimination,
	MostBonusUsed
};

USTRUCT()
struct FPlayerTrackedData
{
	GENERATED_BODY()

public:
	FPlayerTrackedData();
	FPlayerTrackedData(const int ID);
	
	int PlayerId = 0;

	int NbOfStealParachute = 0;
	float TimeWithParachute = 0.f;
	int NbOfElimination = 0;
	int NbBonusUsed = 0;
};


UCLASS()
class FREEFALLINGCOUCHGAME_API ATrackerActor : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	TArray<FPlayerTrackedData> PlayerTrackedDataList;
	UPROPERTY()
	TObjectPtr<AParachute> CurrentParachute;
	
public:
	TArray<int> GetTrackingWinners(ETrackingRewardCategory Category);
	
	void Init(AParachute* ParachuteInstance, TArray<AFreeFallCharacter*>& CharactersInsideArena);
	void Tick(float DeltaTime) override;

	UFUNCTION()
	void TrackHoldParachute(float DeltaTime);
	
	UFUNCTION()
	void AddNbOfStealParachute(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NewOwner);
	
	UFUNCTION()
	void AddNbOfElimination(const AFreeFallCharacter* Character);
	
	UFUNCTION()
	void AddNbBonusUsed(const AFreeFallCharacter* Character);

	UFUNCTION()
	void DebugPrintResultReward();
	
private:
	UPROPERTY()
	int CurrentParachuteHolderIndex;
	
	UFUNCTION()
	void StartParachuteTracking(AFreeFallCharacter* NewOwner);
	UFUNCTION()
	void ChangeParachuteTracking(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NewOwner);
	UFUNCTION()
	void StopParachuteTracking(AFreeFallCharacter* PreviousOwner);
};
