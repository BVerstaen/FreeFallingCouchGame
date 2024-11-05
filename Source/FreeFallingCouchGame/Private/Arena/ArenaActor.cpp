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

	OffScreenHorizontalTolerance = 0.f;
	OffScreenVerticalTolerance = 0.f;
	NearEdgeScreenTolerance = 0.f;
}

void AArenaActor::Init(const AFreeFallGameMode* FreeFallGameMode)
{
	//Set existing characters
	if(FreeFallGameMode == nullptr) return;
	if(FreeFallGameMode->CharactersInsideArena.Num() <= 0) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "No Existing Character in Arena");
	
	CharactersInsideArena.Empty();
	CharactersInsideArena.Append(FreeFallGameMode->CharactersInsideArena);

	//Set off-screen tolerance
	const UCharactersSettings* CharactersSettings = GetDefault<UCharactersSettings>();
	OffScreenHorizontalTolerance = CharactersSettings->MarginHorizontalOffScreen;
	OffScreenVerticalTolerance = CharactersSettings->MarginVerticalOffScreen;
	NearEdgeScreenTolerance = CharactersSettings->PercentageCloseEdgeScreen;
	
	MainCameraController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	
	Parachute = FreeFallGameMode->GetParachuteInstance();

	CameraMain = FindCameraByTag("CameraMain");
	if(!CameraMain)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Can't find the main camera! Add \"CameraMain\" tag to main camera!");
}

UCameraComponent* AArenaActor::FindCameraByTag(const FName& Tag) const
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);
	
	//Go through each found actor and return the first UCameraComponent
	for (AActor* FoundActor : FoundActors)
	{
		if (FoundActor)
		{
			if (UCameraComponent* FoundCameraComponent = FoundActor->FindComponentByClass<UCameraComponent>())
			{
				return FoundCameraComponent;
			}
		}
	}
	
	return nullptr;
}

void AArenaActor::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
{
	//Find Viewport
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient == nullptr) return;

	FViewport* Viewport = ViewportClient->Viewport;
	if (Viewport == nullptr) return;

	//Calculate Viewport Rect according to Camera Aspect Ratio and Viewport ViewRect
	FIntRect ViewRect(
		Viewport->GetInitialPositionXY(),
		Viewport->GetInitialPositionXY() + Viewport->GetSizeXY()
	);

	
	FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	//Fill Output parameters with Viewport Rect
	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;

	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;
}

// Called every frame
void AArenaActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckAndRemoveOutOfBoundPlayers();
}

void AArenaActor::CheckAndRemoveOutOfBoundPlayers()
{
	TArray<AFreeFallCharacter*> CharactersToRemove;
	
	//Check if character was rendered on screen (inverted loop to avoid indexation problems)
	for(int i = CharactersInsideArena.Num() - 1; i > -1; i--)
	{
		AFreeFallCharacter* Character = CharactersInsideArena[i];
		if(Character == nullptr) continue;
		
		FVector PlayerLocation = Character->GetActorLocation();
		FVector2D ScreenPosition;
		
		//Check if player position is a valid screen position
		bool bCanConvertToScreen = MainCameraController->ProjectWorldLocationToScreen(PlayerLocation, ScreenPosition);
		if(bCanConvertToScreen)
		{
			//Check if player is beyond margin Tolerance
			FVector2D ViewportSizeMin;
			FVector2D ViewportSizeMax;
			GetViewportBounds(ViewportSizeMin, ViewportSizeMax);
			
			if (IsOutOfBounds(ScreenPosition, ViewportSizeMin, ViewportSizeMax))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Character->GetName() + "is out");

				//Destroy current player
				if(OnCharacterDestroyed.IsBound())
					OnCharacterDestroyed.Broadcast(Character);

				CharactersToRemove.Add(Character);
			}
			else if (IsNearOutOfBounds(ScreenPosition, ViewportSizeMin, ViewportSizeMax))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Is Near Edge of Screen");
			}
		}
	}

	//Remove & destroy actors
	for (AFreeFallCharacter* Character : CharactersToRemove)
	{
		CharactersInsideArena.Remove(Character);
		Character->DestroyPlayer();
	}
	CharactersToRemove.Empty();
}


void AArenaActor::CheckOutOfBoundParachute()
{
	FVector ParachuteLocation = Parachute->GetActorLocation();
	FVector2D ScreenPosition;
	//Check if player position is a valid screen position
	bool bCanConvertToScreen = MainCameraController->ProjectWorldLocationToScreen(ParachuteLocation, ScreenPosition);

	if(!bCanConvertToScreen) return;

	FVector2D ViewportSizeMin;
	FVector2D ViewportSizeMax;
	GetViewportBounds(ViewportSizeMin, ViewportSizeMax);
	
	if(IsOutOfBounds(ScreenPosition, ViewportSizeMin, ViewportSizeMax))
	{
		//Parachute->RecenterParachute();
	}
}

bool AArenaActor::IsOutOfBounds(FVector2D ScreenPosition, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax) const
{
	return !(ScreenPosition.X >= -OffScreenHorizontalTolerance + ViewportSizeMin.X && ScreenPosition.X <= ViewportSizeMax.X + OffScreenHorizontalTolerance &&
				ScreenPosition.Y >= -OffScreenVerticalTolerance + ViewportSizeMin.Y && ScreenPosition.Y <= ViewportSizeMax.Y + OffScreenVerticalTolerance);
}

bool AArenaActor::IsNearOutOfBounds(FVector2D ScreenPosition, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax) const
{
	FVector2D ViewportSize = ViewportSizeMax - ViewportSizeMin;
	return !(ScreenPosition.X >= ViewportSize.X * NearEdgeScreenTolerance && ScreenPosition.X <= ViewportSize.X - ViewportSize.X * NearEdgeScreenTolerance &&
				ScreenPosition.Y >= ViewportSize.Y * NearEdgeScreenTolerance && ScreenPosition.Y <= ViewportSize.Y - ViewportSize.Y * NearEdgeScreenTolerance);
}
