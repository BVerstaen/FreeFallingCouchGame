// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
#include "FreeFallCharacterStateFastDive.generated.h"

enum class EDiveLayersID : uint8;
class ADiveLevels;
/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterStateFastDive : public UFreeFallCharacterState
{
	GENERATED_BODY()

	virtual EFreeFallCharacterStateID GetStateID() override;

	virtual void StateInit(UFreeFallCharacterStateMachine* InStateMachine) override;

	virtual void StateEnter(EFreeFallCharacterStateID PreviousStateID) override;

	virtual void StateExit(EFreeFallCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;


#pragma region Mesh movement

protected:
	/*Rotation maximum en Roll du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementRotationAngle;
	
	/*Vitesse de rotation du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementDampingSpeed;
	
#pragma endregion

private:
	UPROPERTY()
	TObjectPtr<ADiveLevels> DiveLevelsActor;

	//Temps nécessaire au joueur pour atteindre le milieu de la Layer visée
	UPROPERTY(EditAnywhere)
	float CrossingLayerTime;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimSequence> DiveAnimation;
	
	UPROPERTY()
	float TargetLayerZCenter;

	//1 or -1, direction of movement;
	UPROPERTY()
	int DirectionScaleValue;

	void CheckTargetedLayer();

	void ExitStateFastDive();

	UPROPERTY()
	EDiveLayersID TargetLayer;
};
