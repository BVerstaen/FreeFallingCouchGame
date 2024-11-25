// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/SoundSubsystem.h"

#include "Kismet/GameplayStatics.h"

UAudioComponent* USoundSubsystem::PlaySound(FName SoundName, const AActor* ParentActor, bool bAttachToActor,
                                float PitchMultiplier, float StartTime)
{
	//Get Sound Data
	const USoundSettings* SoundSettings = GetDefault<USoundSettings>();
	const FSoundListData* SoundData = SoundSettings->SoundLists.Find(SoundName);
	if(!SoundData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Sound can't be found ! Be sure the sound exist in Sound Settings !");
		return nullptr;
	}

	if(SoundData->Sounds.Num() <= 0)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Sound List Empty !");
		return nullptr;
	}
	
	//Load Sound Cue
	USoundCue* Cue = SoundData->Sounds[FMath::RandRange(0, SoundData->Sounds.Num() - 1)].LoadSynchronous();

	//Play sound cue
	if(!Cue)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cue can't be found !");
		return nullptr;
	}

	if(bAttachToActor)
	{
		return UGameplayStatics::SpawnSoundAttached(Cue, ParentActor->GetRootComponent(), FName("NAME_None"), FVector(ForceInit),
			FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, false,
			SoundVolume, PitchMultiplier, StartTime);
	}
	
	return UGameplayStatics::SpawnSoundAtLocation(GetWorld(), Cue, ParentActor->GetActorLocation(), FRotator::ZeroRotator,
		SoundVolume, PitchMultiplier, StartTime);
}

void USoundSubsystem::Play2DSound(FName SoundName)
{
	//Get Sound Data
	const USoundSettings* SoundSettings = GetDefault<USoundSettings>();
	const FSoundListData* SoundData = SoundSettings->SoundLists.Find(SoundName);
	if(!SoundData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Sound can't be found ! Be sure the sound exist in Sound Settings !");
		return;
	}
	
	//Load Sound Cue
	USoundCue* Cue = SoundData->Sounds[FMath::RandRange(0, SoundData->Sounds.Num() - 1)].LoadSynchronous();
	
	//Play sound cue
	if(Cue)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), Cue);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cue can't be found !");
	}
}

void USoundSubsystem::PlayMusic(FName MusicName, bool bLoop, float PitchMultiplier,
	float StartTime)
{
	const USoundSettings* SoundSettings = GetDefault<USoundSettings>();
	USoundCue* Cue = SoundSettings->MusicLists.Find(MusicName)->LoadSynchronous();

	//Play music cue
	if(!Cue)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Cue can't be found !");
		return;
	}

	UGameplayStatics::PlaySound2D(GetWorld(), Cue);
}
