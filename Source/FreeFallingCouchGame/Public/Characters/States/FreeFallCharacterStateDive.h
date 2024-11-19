// Fill out your copyright notice in the Description page of Project Settings.

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
	//void ApplyDiveForce();

	void ApplyMoveInputs(float DeltaTime);

	//bool IsInCenterOfLayer() const;

	FString GetLayerName(EDiveLayersID LayerID) const;

protected:
	
	UPROPERTY(EditAnywhere)
	float DiveSpeed = 400.f;

	UPROPERTY(EditAnywhere)
	float CrossLayerCooldown = 0.5f;

	UPROPERTY(EditAnywhere)
	float DiveLayerThreshold = 10.f;

#pragma region MoveInputs

public:
	void SetMoveStat(float Move_AccelerationSpeed);
	
protected:
	UPROPERTY()
	float AccelerationSpeed;

	/*Le seuil à partir duquel le joueur ne bloque plus sa rotation et permet d'être influencé (uniquement si attrape joueur)*/
	UPROPERTY()
	float OrientationThreshold;

	FVector2D OldInputDirection;

private:
	float* AccelerationAlpha;

#pragma endregion

#pragma region Mesh movement
	
protected:
	/*Rotation maximum en Roll du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementRotationAngle;
	
	/*Vitesse de rotation du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementDampingSpeed;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DiveDownwardsAnimation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DiveUpwardsAnimation;
	
#pragma endregion
	
private:
	UPROPERTY()
	float EnterDiveLevelThreshold = 5.f;
	
	UPROPERTY()
	TObjectPtr<ADiveLevels> DiveLevelsActor;

	UPROPERTY()
	EDivePhase CurrentDivePhase;

	UPROPERTY()
	EDiveLayersID TargetLayer;

	UPROPERTY()
	float CrossLayerClock;

	float OldInputDive;
	
private:
	UFUNCTION()
	void OnInputFastDive();
};
