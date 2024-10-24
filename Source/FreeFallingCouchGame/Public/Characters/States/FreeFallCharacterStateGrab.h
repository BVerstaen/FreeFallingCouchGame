// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "FreeFallCharacterStateGrab.generated.h"

/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterStateGrab : public UFreeFallCharacterState
{
	GENERATED_BODY()

public:
	virtual EFreeFallCharacterStateID GetStateID() override;

	virtual void StateInit(UFreeFallCharacterStateMachine* InStateMachine) override;
	
	virtual void StateEnter(EFreeFallCharacterStateID PreviousStateID) override;
	
	void ExitStateConditions() const;

protected:
	AFreeFallCharacter* FindPlayerToGrab() const;

	UPROPERTY(EditAnywhere, Category="PlayerGrab - Catch")
	float GrabForwardDistance = 1.0f;
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Catch")
	float GrabRadius = 10.0f;
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Influences")
	float RotationSpeed = 10.0f;
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Influences")
	float RotationInfluenceStrength = 5.f;
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Launch")
	float LaunchOtherCharacterForceMultiplier = 20.f;
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Launch")
	float LaunchOtherCharacterBaseLaunchMultiplier = 0.f;
public:
	UPROPERTY()
	UPhysicsConstraintComponent* GrabConstraint;

};
