// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FreeFallCharacterStateMove.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterStateMove : public UFreeFallCharacterState
{
	GENERATED_BODY()

public:
	virtual EFreeFallCharacterStateID GetStateID() override;

	virtual void StateEnter(EFreeFallCharacterStateID PreviousStateID) override;

	virtual void StateExit(EFreeFallCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere)
	float MaxMoveSpeed;

	UPROPERTY(EditAnywhere)
	float StartMoveSpeed;

	UPROPERTY(EditAnywhere)
	float ReachMaxSpeedTime;

private:
	float AccelerationAlpha;
};
