// Fill out your copyright notice in the Description page of Project Settings.


#include "Haptic/HapticsSubsystem.h"

UHapticsHandler* UHapticsSubsystem::GetHaptics() const
{
	if(InstanceHaptics->IsValidLowLevel())
	{
		return InstanceHaptics;
	} else
	{
		return nullptr;
	}
}

void UHapticsSubsystem::InitHaptics(TSubclassOf<UHapticsHandler> Template)
{
	if(InstanceHaptics->IsValidLowLevel())
		return;
	//InstanceHaptics = NewObject<UHapticsHandler>(Template);
	InstanceHaptics = NewObject<UHapticsHandler>(this, Template); 
	UE_LOG(LogTemp, Warning, TEXT("Object class: %s"), *InstanceHaptics->GetClass()->GetName());

	// set owner
	//const TCHAR* wavLink = *InstanceHaptics->GetName();
	//InstanceHaptics->Rename(wavLink, this);
}
