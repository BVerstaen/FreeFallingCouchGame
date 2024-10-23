// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/ArenaActor.h"

#include "Characters/FreeFallCharacter.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/CharactersSettings.h"


// Sets default values
AArenaActor::AArenaActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OffScreenTolerance = 0.f;
	NearEdgeScreenTolerance = 0.f;
}

void AArenaActor::Init()
{
	//Set existing characters
	AFreeFallGameMode* FreeFallGameMode = Cast<AFreeFallGameMode>(GetWorld()->GetAuthGameMode());
	if(FreeFallGameMode == nullptr) return;
	if(FreeFallGameMode->CharactersInsideArena.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No Existing Character in Arena");
	
	CharactersInsideArena.Empty();
	CharactersInsideArena.Append(FreeFallGameMode->CharactersInsideArena);

	//Set off-screen tolerance
	const UCharactersSettings* CharactersSettings = GetDefault<UCharactersSettings>();
	OffScreenTolerance = CharactersSettings->MarginAliveOffScreen;
	NearEdgeScreenTolerance = CharactersSettings->PercentageCloseEdgeScreen;
}

// Called every frame
void AArenaActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AFreeFallCharacter*> CharactersToRemove;
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	//Check if character was rendered on screen
	for(AFreeFallCharacter* Character : CharactersInsideArena)
	{
		if(Character == nullptr) continue;

		FVector PlayerLocation = Character->GetActorLocation();
		FVector2D ScreenPosition;
		
		//Check if player position is a valid screen position
		bool bCanConvertToScreen = PlayerController->ProjectWorldLocationToScreen(PlayerLocation, ScreenPosition);
		if(bCanConvertToScreen)
		{
			//Check if player is beyond margin Tolerance
			const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());

			if (!(ScreenPosition.X >= -OffScreenTolerance && ScreenPosition.X <= ViewportSize.X + OffScreenTolerance &&
				ScreenPosition.Y >= -OffScreenTolerance && ScreenPosition.Y <= ViewportSize.Y + OffScreenTolerance))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Character->GetName() + "is out");

				//Destroy current player
				if(OnCharacterDestroyed.IsBound())
					OnCharacterDestroyed.Broadcast(Character);
			
				CharactersToRemove.Add(Character);
				Character->Destroy();
			}
			else if (!(ScreenPosition.X >= ViewportSize.X * NearEdgeScreenTolerance && ScreenPosition.X <= ViewportSize.X - ViewportSize.X * NearEdgeScreenTolerance &&
				ScreenPosition.Y >= ViewportSize.Y * NearEdgeScreenTolerance && ScreenPosition.Y <= ViewportSize.Y - ViewportSize.Y * NearEdgeScreenTolerance))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Is Near Edge of Screen");
			}
		}
	}

	//Check if there's any characters to remove
	if(CharactersToRemove.Num() > 0)
	{
		for(AFreeFallCharacter* Character : CharactersToRemove)
			CharactersInsideArena.Remove(Character);
		CharactersToRemove.Empty();
	}
}
