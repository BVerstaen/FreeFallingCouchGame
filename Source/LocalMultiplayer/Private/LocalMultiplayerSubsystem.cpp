// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
	int NumberOfControllers = LocalMultiplayerSettings->GetNbKeyboardProfiles() + LocalMultiplayerSettings->NbMaxGamepad;

	CurrentMappingType = MappingType;

	//Create PC keyboard & gamepad
	for(int i=0; i < NumberOfControllers; i++)
	{
		UGameplayStatics::CreatePlayer(GetWorld(),-1);
	}

	//Connect existing assigned keyboard players
	for (const TPair<int, int> ConnectedKeyboard : PlayerIndexFromKeyboardProfileIndex)
	{
		AssignKeyboardMapping(ConnectedKeyboard.Key, ConnectedKeyboard.Value, MappingType);
	}

	//Connect existing assigned gamepad players
	for (const TPair<int, int> ConnectedGamepadProfileIndex : PlayerIndexFromGamepadProfileIndex)
	{
		AssignGamepadInputMapping(ConnectedGamepadProfileIndex.Key, MappingType);
	} 
}

void ULocalMultiplayerSubsystem::ErasePlayers()
{
	PlayerIndexFromGamepadProfileIndex.Empty();
	PlayerIndexFromKeyboardProfileIndex.Empty();
	LastAssignedPlayerIndex = -1;
}


int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int keyboardProfileIndex)
{
	if(PlayerIndexFromKeyboardProfileIndex.Contains(keyboardProfileIndex))
	{
		return PlayerIndexFromKeyboardProfileIndex[keyboardProfileIndex];
	}
	return -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex)
{
	LastAssignedPlayerIndex++;
	PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, LastAssignedPlayerIndex);

	//GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "LAPI :" + FString::FromInt(LastAssignedPlayerIndex));
	return LastAssignedPlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex, ELocalMultiplayerInputMappingType MappingType) const
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();

	APlayerController* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex);
	if(LocalPlayer == nullptr) return;
	UEnhancedInputLocalPlayerSubsystem* PlayerSubsystem = LocalPlayer->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(PlayerSubsystem == nullptr) return;
	
	UInputMappingContext* IMC = LocalMultiplayerSettings->KeyboardProfilesData[KeyboardProfileIndex].GetIMCFromType(MappingType);
	if(IMC == nullptr) return;


	//GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "Assign :" + FString::FromInt(KeyboardProfileIndex));
	//GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "To :" + FString::FromInt(PlayerIndex));
	
	FModifyContextOptions FModify;
	FModify.bForceImmediately = true;
	PlayerSubsystem->AddMappingContext(IMC, 1, FModify);
}

int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromGamepadDeviceID(int DeviceID)
{
	if(PlayerIndexFromGamepadProfileIndex.Contains(DeviceID))
	{
		return PlayerIndexFromGamepadProfileIndex[DeviceID];
	}
	return -1;
}

int ULocalMultiplayerSubsystem::AssignNewPlayerToGamepadDeviceID(int DeviceID)
{
	LastAssignedPlayerIndex++;
	PlayerIndexFromGamepadProfileIndex.Add(DeviceID, LastAssignedPlayerIndex);

	//GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "LAPI :" + FString::FromInt(LastAssignedPlayerIndex));
	return LastAssignedPlayerIndex;
}

void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex, ELocalMultiplayerInputMappingType MappingType) const
{
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();

	APlayerController* LocalPlayer = UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex);
	UEnhancedInputLocalPlayerSubsystem* PlayerSubsystem = LocalPlayer->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(PlayerSubsystem == nullptr) return;
	
	UInputMappingContext* IMC = LocalMultiplayerSettings->GamepadProfileData.GetIMCFromType(MappingType);
	if(IMC == nullptr) return;
	
	FModifyContextOptions FModify;
	FModify.bForceImmediately = true;
	PlayerSubsystem->AddMappingContext(IMC, 1, FModify);
}

void ULocalMultiplayerSubsystem::SetCanCreateNewPlayer(bool canCreate)
{
	bCanCreateNewPlayer = canCreate;
}
