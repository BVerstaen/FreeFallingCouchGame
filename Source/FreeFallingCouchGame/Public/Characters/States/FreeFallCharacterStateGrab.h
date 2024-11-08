// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
#include "Obstacle/Obstacle.h"
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
	AActor* FindActorToGrab() const;

	void ReleasePlayerGrab(EFreeFallCharacterStateID PreviousStateID);
	void ReleaseObjectGrab(EFreeFallCharacterStateID PreviousStateID);
	void PlayerGrab() const;
	void ObjectGrab() const;
	
	
	/*Distance de la boule (par rapport au forward du joueur) qui check si on peut grab un joueur ou obstacle*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float GrabForwardDistance = 1.0f;
	/*Le radius de la boule qui check si on peut grab un joueur ou obstacle*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float GrabRadius = 10.0f;
	/*Le radius de la boule qui check si on peut grab un joueur ou obstacle*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float GrabMinimumDistance = 30.0f;
	
	/*A quelle vitesse celui qui est agrippé suit la rotation du "maitre" ?*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Influences")
	float RotationSpeed = 10.0f;
	/*La force d'influence de rotation*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Influences")
	float RotationInfluenceStrength = 5.f;
	
	/*Multiplicateur de vélocité quand le joueur lance sa prise (force ajoutée avec celle par défaut)*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Launch")
	float LaunchOtherCharacterForceMultiplier = 20.f;
	/*Multiplicateur de base quand le joueur lance sa prise (force par défaut si immobile)*/
	UPROPERTY(EditAnywhere, Category="PlayerGrab - Launch")
	float LaunchOtherCharacterBaseLaunchMultiplier = 0.f;
	
	/*Multiplicateur de vélocité quand le joueur lance sa prise (pour les objets) (force ajoutée avec celle par défaut)*/
	UPROPERTY(EditAnywhere, Category="ObjectGrab - Launch")
	float LaunchObjectForceMultiplier = 20.f;
	/*Multiplicateur de base quand le joueur lance sa prise (pour les objets)  (force par défaut si immobile)*/
	UPROPERTY(EditAnywhere, Category="ObjectGrab - Launch")
	float LaunchObjectBaseLaunchMultiplier = 0.f;
	
	UPROPERTY()
	EFreeFallCharacterStateID PreviousState;

};
