// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FreeFallCharacterStateMachine.h"

#include "FreeFallCharacterState.h"
#include "Characters/FreeFallCharacter.h"
#include "Characters/FreeFallCharacterStateID.h"
#include "Settings/CharactersSettings.h"

void UFreeFallCharacterStateMachine::Init(AFreeFallCharacter* InCharacter)
{
	Character = InCharacter;
	CreateStates();
	InitStates();

	ChangeState(EFreeFallCharacterStateID::Idle);
}

void UFreeFallCharacterStateMachine::Tick(float DeltaTime)
{
	if (CurrentState == nullptr) return;
	CurrentState->StateTick(DeltaTime);
}

AFreeFallCharacter* UFreeFallCharacterStateMachine::GetCharacter() const
{
	return Character;
}

void UFreeFallCharacterStateMachine::ChangeState(EFreeFallCharacterStateID NextStateID)
{
	UFreeFallCharacterState* NextState = GetState(NextStateID);
	//Do nothing if next state is not found
	if (NextState == nullptr) return;

	if (CurrentState != nullptr)
	{
		CurrentState->StateExit(NextStateID);
	}

	EFreeFallCharacterStateID PreviousStateID = CurrentStateID;
	CurrentStateID = NextStateID;
	CurrentState = NextState;

	if (CurrentState != nullptr)
	{
		CurrentState->StateEnter(PreviousStateID);
	}
}

UFreeFallCharacterState* UFreeFallCharacterStateMachine::GetState(EFreeFallCharacterStateID StateID)
{
	for (UFreeFallCharacterState* State : AllStates)
	{
		if (State == nullptr) continue;
		if (State->GetStateID() == StateID)
			return State;
	}
	return nullptr;
}

void UFreeFallCharacterStateMachine::CreateStates()
{
	const UCharactersSettings* SmashCharacterSettings = GetDefault<UCharactersSettings>();
	TMap<EFreeFallCharacterStateID, TSubclassOf<UFreeFallCharacterState>> StatesFromSettings = SmashCharacterSettings->SmashCharacterStates;
	TMap<EFreeFallCharacterStateID, TSubclassOf<UFreeFallCharacterState>> StatesOverrides = Character->FreeFallCharacterStatesOverride;
	
	for (TTuple<EFreeFallCharacterStateID, TSubclassOf<UFreeFallCharacterState>> StateFromSetting : StatesFromSettings)
	{
		if (StateFromSetting.Key == EFreeFallCharacterStateID::None) continue;
		
		TSubclassOf<UFreeFallCharacterState>* State;
		
		if (StatesOverrides.Contains(StateFromSetting.Key)) State = &StatesOverrides[StateFromSetting.Key];
		else State = &StateFromSetting.Value;
		
		if (State == nullptr) continue;

		UFreeFallCharacterState* StateObject = NewObject<UFreeFallCharacterState>(this, *State);
		AllStates.Add(StateObject);
	}
}

void UFreeFallCharacterStateMachine::InitStates()
{
	for (UFreeFallCharacterState* State : AllStates)
	{
		State->StateInit(this);
	}
}
