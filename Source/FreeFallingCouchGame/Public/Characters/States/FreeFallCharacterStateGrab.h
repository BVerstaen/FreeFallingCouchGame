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

	/*Distance de la boule (par rapport au forward du joueur) qui check si on peut grab un joueur*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Catch")
	float GrabForwardDistance = 1.0f;
	/*Le radius de la boule qui check si on peut grab un joueur*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Catch")
	float GrabRadius = 10.0f;
	/*A quelle vitesse celui qui est agrippé suit la rotation du "maitre" ?*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Influences")
	float RotationSpeed = 10.0f;
	/*La force d'influence*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Influences")
	float RotationInfluenceStrength = 5.f;
	/*Multiplicateur de vélocité quand le joueur lance sa prise (force ajoutée avec celle par défaut)*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Launch")
	float LaunchOtherCharacterForceMultiplier = 20.f;
	/*Multiplicateur de base quand le joueur lance sa prise (force par défaut si immobile)*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Launch")
	float LaunchOtherCharacterBaseLaunchMultiplier = 0.f;
public:
	UPROPERTY()
	UPhysicsConstraintComponent* GrabConstraint;

};
