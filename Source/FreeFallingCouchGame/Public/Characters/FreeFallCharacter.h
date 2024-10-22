// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FreeFallCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UFreeFallCharacterInputData;
class UFreeFallCharacterState;
enum class EFreeFallCharacterStateID : uint8;
class UFreeFallCharacterStateMachine;

UCLASS()
class FREEFALLINGCOUCHGAME_API AFreeFallCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Unreal Default
public:
	// Sets default values for this character's properties
	AFreeFallCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma endregion

#pragma region StateMachine
public:
	void CreateStateMachine();

	void InitStateMachine();

	void TickStateMachine(float DeltaTime) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFreeFallCharacterStateMachine> StateMachine;

public:
	TMap<EFreeFallCharacterStateID, TSubclassOf<UFreeFallCharacterState>> FreeFallCharacterStatesOverride;

#pragma endregion

#pragma region Input Data / Mapping Context

public:
	UPROPERTY()
	TObjectPtr<UFreeFallCharacterInputData> InputData;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> InputMappingContext;

protected:
	void SetupMappingContextIntoController() const;

#pragma endregion

#pragma region Input Move

public:
	FVector2D GetInputMove() const;

protected:
	UPROPERTY()
	FVector2D InputMove = FVector2D::ZeroVector;
private:
	void BindInputMoveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputMove(const FInputActionValue& Value);
	
#pragma endregion

#pragma region Input Dive

public:
	float GetInputDive() const;
	float GetDefaultZPosition() const;

protected:
	UPROPERTY()
	float InputDive = 0.f;

	UPROPERTY()
	float DefaultZPosition = 0.f;
	
private:
	void BindInputDiveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputDive(const FInputActionValue& Value);

#pragma endregion

#pragma region IDPlayer
protected:
	uint8 ID_PlayerLinked = -1;
public:
	int getIDPlayerLinked() const { return ID_PlayerLinked; }
	void setIDPlayerLinked(int InID) { ID_PlayerLinked = InID; }
#pragma endregion
};
