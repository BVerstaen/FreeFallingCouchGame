// Fill out your copyright notice in the Description page of Project Settings.


#include "Settings/SoundSettings.h"
#include "Kismet/GameplayStatics.h"


void USoundSettings::PlaySound(FName SoundName, AActor* ParentActor, bool bAttachToActor, float VolumeMultiplier,
	float PitchMultiplier, float StartTime)
{
	//Get Sound Data
	FSoundListData* SoundData = SoundLists.Find(SoundName);
	if(!SoundData) return;

	//Load Sound Cue
	USoundCue* Cue = SoundData->Sounds[FMath::RandRange(0, SoundData->Sounds.Num() - 1)].LoadSynchronous();

	//Play sound cue
	if(!Cue) return;

	if(bAttachToActor)
	{
		UGameplayStatics::SpawnSoundAttached(Cue, ParentActor->GetRootComponent(), FName("NAME_None"), FVector(ForceInit),
			FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, false,
			VolumeMultiplier, PitchMultiplier, StartTime);
	}
	else
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Cue, ParentActor->GetActorLocation(), FRotator::ZeroRotator,
			VolumeMultiplier, PitchMultiplier, StartTime);
	}
	UGameplayStatics::PlaySound2D(GetWorld(), Cue);
}

void USoundSettings::PlaySound2D(FName SoundName)
{
	//Get Sound Data
	FSoundListData* SoundData = SoundLists.Find(SoundName);
	if(!SoundData) return;

	//Load Sound Cue
	USoundCue* Cue = SoundData->Sounds[FMath::RandRange(0, SoundData->Sounds.Num() - 1)].LoadSynchronous();

	//Play sound cue
	if(Cue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Cue);
	}
}
