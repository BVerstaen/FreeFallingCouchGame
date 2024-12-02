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
	bool PlayerGrab() const;
	bool ObjectGrab() const;
	
	
	/*Distance de la boule (par rapport au forward du joueur) qui check si on peut grab un joueur ou obstacle*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float GrabForwardDistance = 1.0f;
	/*Le radius de la boule qui check si on peut grab un joueur ou obstacle*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float GrabRadius = 10.0f;

	/*Distance minimum avec lequel un joueur n'est pas recenter*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float GrabMinimumDistance = 30.0f;
	
	/*Distance maximum avec lequel un joueur n'est pas recenter*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float GrabMaximumDistance = 200.0f;

	/*Angle limite entre le joueur et celui qui le grab à partir duquel on ne peut pas grab
	 *car trop proche (je suis fatigué déso si pas clair)*/
	UPROPERTY(EditAnywhere, Category="Grab - Catch")
	float ChainGrabAngleLimit = 20.0f;
	
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

	UPROPERTY(EditAnywhere, Category="Grab - Animation")
	TObjectPtr<UAnimSequence> StartGrabAnimation;

	UPROPERTY(EditAnywhere, Category="Grab - Animation")
	TObjectPtr<UAnimSequence> EndGrabAnimation;
	
	UPROPERTY()
	EFreeFallCharacterStateID PreviousState;


	
};
