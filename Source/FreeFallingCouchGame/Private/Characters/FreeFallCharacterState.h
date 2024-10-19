// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FreeFallCharacterState.generated.h"

class UCharactersSettings;
class UFreeFallCharacterStateMachine;
class AFreeFallCharacter;
enum class EFreeFallCharacterStateID : uint8;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterState : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFreeFallCharacterState();

	virtual EFreeFallCharacterStateID GetStateID();

	virtual void StateInit(UFreeFallCharacterStateMachine* InStateMachine);

	virtual void StateEnter(EFreeFallCharacterStateID PreviousStateID);

	virtual void StateTick(float DeltaTime);

	virtual void StateExit(EFreeFallCharacterStateID NextStateID);

protected:
	UPROPERTY()
	TObjectPtr<AFreeFallCharacter> Character;
	
	UPROPERTY()
	TObjectPtr<UFreeFallCharacterStateMachine> StateMachine;

	UPROPERTY()
	const UCharactersSettings* CharactersSettings;
};
