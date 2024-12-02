// Fill out your copyright notice in the Description page of Project Settings.


#include "Haptic/HapticsStatics.h"
#include "Engine/World.h"
#include "Haptic/HapticsSubsystem.h"

void UHapticsStatics::CallHapticsCollision(UObject* WorldContextObject, APlayerController *PlayerController)
{
	UWorld* World = WorldContextObject->GetWorld();
	UGameInstance* GameInstance = World->GetGameInstance();
	UHapticsSubsystem* HapticsSubsystem = GameInstance->GetSubsystem<UHapticsSubsystem>();
	UHapticsHandler* HapticsHandler = HapticsSubsystem->GetHaptics();
	HapticsHandler->CallHapticsSingle(PlayerController);
	
	//GEngine->GetWorld()->GetGameInstance()->GetSubsystem<UHapticsSubsystem>()->GetHaptics()->ReceiveCallHapticsCollision(PlayerController);
	//GEngine->GetWorld()->GetGameInstance()->GetSubsystem<UHapticsSubsystem>()->GetHaptics()->ReceiveCallHapticsCollision(PlayerController);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Haptics collision");
}

void UHapticsStatics::CallHapticsAll(UObject* WorldContextObject)
{
	WorldContextObject->GetWorld()->GetGameInstance()->GetSubsystem<UHapticsSubsystem>()->GetHaptics()->CallHapticsMulti();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Haptics all");
}
