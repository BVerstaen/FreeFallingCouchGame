﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
#include "FreeFallCharacterStateDive.generated.h"

enum class EDiveLayersID : uint8;
class ADiveLevels;

UENUM()
enum class EDivePhase : uint8
{
	ChangingLayer,
	CrossingLayer,
	DiveForcesApplying,
};

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterStateDive : public UFreeFallCharacterState
{
	GENERATED_BODY()

	virtual EFreeFallCharacterStateID GetStateID() override;

	virtual void StateInit(UFreeFallCharacterStateMachine* InStateMachine) override;

	virtual void StateEnter(EFreeFallCharacterStateID PreviousStateID) override;

	virtual void StateExit(EFreeFallCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

	void CheckTargetedLayer();

private:
	void ApplyDiveForce();

	bool IsInCenterOfLayer() const;

	FString GetLayerName(EDiveLayersID LayerID) const;

protected:
	
	UPROPERTY(EditAnywhere)
	float DiveSpeed = 400.f;

	UPROPERTY(EditAnywhere)
	float CrossLayerCooldown = 0.5f;

private:
	UPROPERTY()
	float EnterDiveLevelThreshold = 5.f;
	
	UPROPERTY()
	ADiveLevels* DiveLevelsActor;

	UPROPERTY()
	EDivePhase CurrentDivePhase;

	UPROPERTY()
	EDiveLayersID TargetLayer;

	UPROPERTY()
	float CrossLayerClock;
};
