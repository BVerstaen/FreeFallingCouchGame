// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
#include "FreeFallCharacterStateIdle.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(FreeFallCharacterState), meta=(BlueprintSpawnableComponent))
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterStateIdle : public UFreeFallCharacterState
{
	GENERATED_BODY()

public:
	virtual EFreeFallCharacterStateID GetStateID() override;

	virtual void StateEnter(EFreeFallCharacterStateID PreviousStateID) override;

	virtual void StateExit(EFreeFallCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

#pragma region Mesh movement
protected:
	/*Vitesse de rotation du joueur lorsqu'il reset sa rotation*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementDampingSpeed;
	
#pragma region  Input Event
	
private:
	UFUNCTION()
	void OnInputGrab();

	UFUNCTION()
	void OnInputUsePowerUp();
	
#pragma endregion 
};
