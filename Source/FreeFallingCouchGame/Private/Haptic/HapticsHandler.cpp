// Fill out your copyright notice in the Description page of Project Settings.


#include "Haptic/HapticsHandler.h"

void UHapticsHandler::CallHapticsSingle(APlayerController *Player)
{
	ReceiveCallHapticsCollision(Player);
}

void UHapticsHandler::CallHapticsMulti()
{
	ReceiveCallHapticsAll();
}
