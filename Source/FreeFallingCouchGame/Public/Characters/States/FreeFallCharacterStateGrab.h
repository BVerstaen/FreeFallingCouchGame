// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/FreeFallCharacterState.h"
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

	virtual void StateEnter(EFreeFallCharacterStateID PreviousStateID) override;

	virtual void StateExit(EFreeFallCharacterStateID NextStateID) override;

	virtual void StateTick(float DeltaTime) override;

protected:
	AFreeFallCharacter* FindPlayerToGrab() const;

	UPROPERTY(EditAnywhere)
	float GrabForwardDistance = 1.0f;
	UPROPERTY(EditAnywhere)
	float GrabRadius = 10.0f;
	
public:
	UPROPERTY()
	TObjectPtr<AFreeFallCharacter> OtherCharacter;

};
