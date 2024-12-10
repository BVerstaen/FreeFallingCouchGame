// Fill out your copyright notice in the Description page of Project Settings.


#include "Arena/ArenaObject.h"

#include "Characters/FreeFallCharacter.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/CharactersSettings.h"
#include "UI/Widgets/CharacterIndicatorWidget.h"


// Sets default values
UArenaObject::UArenaObject()
{
	OffScreenHorizontalTolerance = 0.f;
	OffScreenVerticalTolerance = 0.f;
	NearEdgeScreenTolerance = 0.f;
}

void UArenaObject::Init(const AFreeFallGameMode* FreeFallGameMode)
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
	
	//Parachute = FreeFallGameMode->GetParachuteInstance();

	CameraMain = FindCameraByTag("CameraMain");
	if(!CameraMain)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Can't find the main camera! Add \"CameraMain\" tag to main camera!");

	CharacterIndicatorWidgets.Empty();
	for (TObjectPtr<AFreeFallCharacter> Character : CharactersInsideArena)
	{
		CreateCharacterIndicatorWidget(CharactersSettings->CharacterIndicatorWidget, Character->getIDPlayerLinked());
	}
}

UCameraComponent* UArenaObject::FindCameraByTag(const FName& Tag) const
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

void UArenaObject::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
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

	FIntRect ViewportRect;
	if(CameraMain)
		ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	//Fill Output parameters with Viewport Rect
	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;

	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;
}

// Called every frame
void UArenaObject::Tick(float DeltaTime)
{
	if (LastFrameNumberWeTicked == GFrameCounter)
		return;

	CheckAndRemoveOutOfBoundPlayers();
	DisplayOffScreenCharacterIndicators();
	LastFrameNumberWeTicked = GFrameCounter;
}

void UArenaObject::CheckAndRemoveOutOfBoundPlayers()
{
TArray<TObjectPtr<AFreeFallCharacter>> CharactersToRemove;
	
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

				if(!Character) continue;
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
		
		if (TObjectPtr<UCharacterIndicatorWidget> CharacterIndicatorWidget = GetCharacterIndicatorWidgetFromPlayerIndex(Character->getIDPlayerLinked()))
			CharacterIndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
		
		Character->DestroyPlayer(ETypeDeath::Side);
	}
	CharactersToRemove.Empty();
}


void UArenaObject::CheckOutOfBoundParachute()
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

bool UArenaObject::IsOutOfBounds(FVector2D ScreenPosition, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax) const
{
	return !(ScreenPosition.X >= -OffScreenHorizontalTolerance + ViewportSizeMin.X && ScreenPosition.X <= ViewportSizeMax.X + OffScreenHorizontalTolerance &&
				ScreenPosition.Y >= -OffScreenVerticalTolerance + ViewportSizeMin.Y && ScreenPosition.Y <= ViewportSizeMax.Y + OffScreenVerticalTolerance);
}

bool UArenaObject::IsNearOutOfBounds(FVector2D ScreenPosition, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax) const
{
	FVector2D ViewportSize = ViewportSizeMax - ViewportSizeMin;
	return !(ScreenPosition.X >= ViewportSize.X * NearEdgeScreenTolerance && ScreenPosition.X <= ViewportSize.X - ViewportSize.X * NearEdgeScreenTolerance &&
				ScreenPosition.Y >= ViewportSize.Y * NearEdgeScreenTolerance && ScreenPosition.Y <= ViewportSize.Y - ViewportSize.Y * NearEdgeScreenTolerance);
}

void UArenaObject::CreateCharacterIndicatorWidget(TSubclassOf<UUserWidget> CharacterIndicatorWidgetClass, int PlayerIndex)
{
	UUserWidget* Widget = CreateWidget(GetWorld(), CharacterIndicatorWidgetClass);
	Widget->AddToViewport();
	Widget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
	Widget->SetVisibility(ESlateVisibility::Hidden);
	UCharacterIndicatorWidget* CharacterIndicatorWidget = Cast<UCharacterIndicatorWidget>(Widget);
	if (!CharacterIndicatorWidget) return;
	CharacterIndicatorWidget->SetPlayerIndex(PlayerIndex);
	CharacterIndicatorWidgets.Add(CharacterIndicatorWidget);
}

void UArenaObject::DisplayOffScreenCharacterIndicators()
{
	FVector2D ViewportBoundsMin;
	FVector2D ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin, ViewportBoundsMax);

	FVector2D ScreenPosition;
	TObjectPtr<UCharacterIndicatorWidget> CharacterIndicatorWidget;
	//Check if player position is a valid screen position
	
	for (TObjectPtr<AFreeFallCharacter> Character : CharactersInsideArena)
	{
		if(!Character) continue;
		
		bool bCanConvertToScreen = MainCameraController->ProjectWorldLocationToScreen(Character->GetActorLocation(), ScreenPosition);
		if(!bCanConvertToScreen) return;

		CharacterIndicatorWidget = GetCharacterIndicatorWidgetFromPlayerIndex(Character->getIDPlayerLinked());
		if (!CharacterIndicatorWidget) return;
		
		if (IsOutsideOfViewport(ScreenPosition, ViewportBoundsMin, ViewportBoundsMax))
		{
			CharacterIndicatorWidget->SetVisibility(ESlateVisibility::Visible);
			CharacterIndicatorWidget->ShowCharacter(ScreenPosition, ViewportBoundsMin, ViewportBoundsMax);
		}
		else if (CharacterIndicatorWidget->GetVisibility() == ESlateVisibility::Visible)
		{
			CharacterIndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

TObjectPtr<UCharacterIndicatorWidget> UArenaObject::GetCharacterIndicatorWidgetFromPlayerIndex(int PlayerIndex)
{
	for (TObjectPtr<UCharacterIndicatorWidget> CharacterIndicatorWidget : CharacterIndicatorWidgets)
	{
		if (CharacterIndicatorWidget->PlayerIndex == PlayerIndex) return CharacterIndicatorWidget;
	}
	return nullptr;
}

bool UArenaObject::IsOutsideOfViewport(FVector2D ScreenPos, FVector2D ViewportBoundsMin, FVector2D ViewportBoundsMax)
{
	return !(ScreenPos.X >= ViewportBoundsMin.X && ScreenPos.X <= ViewportBoundsMax.X &&
				ScreenPos.Y >= ViewportBoundsMin.Y && ScreenPos.Y <= ViewportBoundsMax.Y);
}
