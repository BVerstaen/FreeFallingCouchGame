// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LocalMultiplayerSettings.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LocalMultiplayerSubsystem.generated.h"

UCLASS()
class LOCALMULTIPLAYER_API ULocalMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType);

	UFUNCTION(BlueprintCallable)
	void ErasePlayers();
	
	int GetAssignedPlayerIndexFromKeyboardProfileIndex(int keyboardProfileIndex);

	int AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex);

	void AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex, ELocalMultiplayerInputMappingType MappingType) const;

	int GetAssignedPlayerIndexFromGamepadDeviceID(int DeviceID);

	int AssignNewPlayerToGamepadDeviceID(int DeviceID);

	void AssignGamepadInputMapping(int PlayerIndex, int DeviceID, ELocalMultiplayerInputMappingType MappingType) const;

protected:
	UPROPERTY()
	int LastAssignedPlayerIndex = -1;

	UPROPERTY()
	TMap<int, int> PlayerIndexFromKeyboardProfileIndex;

	UPROPERTY()
	TMap<int, int> PlayerIndexFromGamepadProfileIndex;

public:
	UPROPERTY()
	bool bCanCreateNewPlayer = true;

	UPROPERTY()
	int NumberOfPlayers = 0;

	UPROPERTY()
	TEnumAsByte<ELocalMultiplayerInputMappingType> CurrentMappingType;

	UFUNCTION(BlueprintCallable)
	void SetCanCreateNewPlayer(bool canCreate);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewPlayerCreated, int, NewPlayerID);
	UPROPERTY(BlueprintAssignable)
	FOnNewPlayerCreated OnNewPlayerCreated;

	UFUNCTION()
	bool IsPlayerLimitReached();
};
