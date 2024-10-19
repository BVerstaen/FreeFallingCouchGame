// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FreeFallCharacterStateMachine.generated.h"

enum class EFreeFallCharacterStateID : uint8;
class UFreeFallCharacterState;
class AFreeFallCharacter;
/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API UFreeFallCharacterStateMachine : public UObject
{
	GENERATED_BODY()

public:
	void Init(AFreeFallCharacter* InCharacter);

	void Tick(float DeltaTime);

	AFreeFallCharacter* GetCharacter() const;

	UFUNCTION(BlueprintCallable)
	void ChangeState(EFreeFallCharacterStateID NextStateID);

	UFreeFallCharacterState* GetState(EFreeFallCharacterStateID StateID);

protected:
	UPROPERTY()
	TObjectPtr<AFreeFallCharacter> Character;

	TArray<UFreeFallCharacterState*> AllStates;

	void CreateStates();

	void InitStates();

	UPROPERTY(BlueprintReadOnly)
	EFreeFallCharacterStateID CurrentStateID;

	UPROPERTY()
	TObjectPtr<UFreeFallCharacterState> CurrentState;
};
