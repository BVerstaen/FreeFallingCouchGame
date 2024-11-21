// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalMultiplayerGameViewportClient.h"
#include "LocalMultiplayerSubsystem.h"
#include "GameFramework/PlayerInput.h"
#include "Kismet/GameplayStatics.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
	Super::PostInitProperties();
	//Deactivate split screen
	ActiveSplitscreenType = ESplitScreenType::Type::None;
	//Setup max number of PC (num players * 2)
	MaxSplitscreenPlayers = 8;
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
	const ULocalMultiplayerSettings* LocalMultiplayerSettings = GetDefault<ULocalMultiplayerSettings>();
	
	bool IsGamepad = EventArgs.IsGamepad();
	if(!IsGamepad)
	{
		int KeyboardProfile = LocalMultiplayerSettings->FindKeyboardProfileIndexFromKey(EventArgs.Key, ELocalMultiplayerInputMappingType::InGame);
		if(KeyboardProfile >= 0)
		{
			//GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "Keyboard Index :" + FString::FromInt(KeyboardProfile));
			int PlayerIndex = LocalMultiplayerSubsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfile);
			if(PlayerIndex < 0 && LocalMultiplayerSubsystem->bCanCreateNewPlayer)
			{
				PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToKeyboardProfile(KeyboardProfile);
				LocalMultiplayerSubsystem->AssignKeyboardMapping(PlayerIndex ,KeyboardProfile, LocalMultiplayerSubsystem->CurrentMappingType);

				//Add new player & call event
				GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "Player Index :" + FString::FromInt(PlayerIndex));
				LocalMultiplayerSubsystem->NumberOfPlayers++;
				LocalMultiplayerSubsystem->OnNewPlayerCreated.Broadcast(PlayerIndex);
			}
			FInputKeyParams Params(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad());
			return UGameplayStatics::GetPlayerController(GetGameInstance()->GetWorld(), PlayerIndex)->InputKey(Params);
		}

	}
	else
	{
		int GamepadID = EventArgs.InputDevice.GetId();
		int PlayerIndex = LocalMultiplayerSubsystem->GetAssignedPlayerIndexFromGamepadDeviceID(GamepadID);
		if(PlayerIndex < 0 && LocalMultiplayerSubsystem->bCanCreateNewPlayer)
		{
			PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(GamepadID);
			LocalMultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, LocalMultiplayerSubsystem->CurrentMappingType);

			GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "GamePad Player Index :" + FString::FromInt(PlayerIndex));

			LocalMultiplayerSubsystem->NumberOfPlayers++;
			LocalMultiplayerSubsystem->OnNewPlayerCreated.Broadcast(PlayerIndex);
			
		}
		FInputKeyParams Params(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad());
		return UGameplayStatics::GetPlayerController(GetGameInstance()->GetWorld(), PlayerIndex)->InputKey(Params);
		
	}
	return Super::InputKey(EventArgs);
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key,
	float Delta, float DeltaTime, int32 NumSamples, bool bGamepad)
{
	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();

	//Can't spawn player if is in game
	if(!LocalMultiplayerSubsystem->bCanCreateNewPlayer) return Super::InputAxis(Viewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
	
	if(bGamepad)
	{
		int GamepadID = InputDevice.GetId();
		int PlayerIndex = LocalMultiplayerSubsystem->GetAssignedPlayerIndexFromGamepadDeviceID(GamepadID);
		if(PlayerIndex < 0 && LocalMultiplayerSubsystem->bCanCreateNewPlayer)
		{
			PlayerIndex = LocalMultiplayerSubsystem->AssignNewPlayerToGamepadDeviceID(GamepadID);
			LocalMultiplayerSubsystem->AssignGamepadInputMapping(PlayerIndex, InGame);
			
			GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Red, "GamePad Player Index :" + FString::FromInt(PlayerIndex));
			
			LocalMultiplayerSubsystem->NumberOfPlayers++;
			LocalMultiplayerSubsystem->OnNewPlayerCreated.Broadcast(PlayerIndex);
		}
		
		return UGameplayStatics::GetPlayerController(GetGameInstance()->GetWorld(), PlayerIndex)->InputAxis(
				Key, Delta, DeltaTime, NumSamples, bGamepad);
	}

	
	return Super::InputAxis(Viewport, InputDevice, Key, Delta, DeltaTime, NumSamples, bGamepad);
}
