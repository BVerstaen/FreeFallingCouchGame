﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
#include "FreeFallCharacterStateDive.generated.h"

class ADiveLevels;
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

private:
	void ApplyDiveForce();

	bool IsInCenterOfLayer() const;

protected:
	UPROPERTY(EditAnywhere)
	float MaxDiveSpeed;

	UPROPERTY(EditAnywhere)
	float StartDiveSpeed;

	UPROPERTY(EditAnywhere)
	float ReachMaxSpeedTime;
	
	UPROPERTY()
	float AccelerationAlpha;
	
	UPROPERTY()
	float ChangeLayerCooldownTime;

	UPROPERTY()
	ADiveLevels* DiveLevelsActor;
};
