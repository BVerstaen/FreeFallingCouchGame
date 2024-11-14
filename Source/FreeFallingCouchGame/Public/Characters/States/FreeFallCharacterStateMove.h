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
	float AccelerationSpeed;
	
	UPROPERTY(EditAnywhere)
	float MaxMoveSpeed;

	UPROPERTY(EditAnywhere)
	float StartMoveSpeed;

	UPROPERTY(EditAnywhere)
	float ReachMaxSpeedTime;

	/*Le seuil à partir duquel le joueur ne bloque plus sa rotation et permet d'être influencé (uniquement si attrape joueur)*/
	UPROPERTY(EditAnywhere, Category="Grab Threshold")
	float OrientationThreshold;
	
	/*Le seuil à partir duquel le joueur ne bloque plus sa rotation et permet d'être influencé (uniquement si attrape joueur)*/
	UPROPERTY(EditAnywhere, Category="Grab Threshold")
	float GrabbedOrientationThreshold;

	/*Le seuil à partir duquel le joueur ne bloque plus sa rotation de risque qu'il colissionne le joueur grab avec le joueur grabbé (uniquement si attrape et attrapée)*/
	UPROPERTY(EditAnywhere, Category="Grab Threshold")
	float GrabToCloseToGrabbedAngle;


#pragma region Mesh movement
	
protected:
	/*Rotation maximum en Yaw du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementRotationAngle;
	
	/*Vitesse de rotation du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementDampingSpeed;

	FVector2D PreviousInputMovement;
	FRotator PreviousRotation = FRotator::ZeroRotator;
	
#pragma endregion
	
private:
	UPROPERTY()
	float AccelerationAlpha;


#pragma region  Input Event
	
private:
	UFUNCTION()
	void OnInputGrab();

	UFUNCTION()
	void OnInputUsePowerUp();

	UFUNCTION()
	void OnInputFastDive();
	
	FVector2D GrabOldInputDirection;
#pragma endregion
};
