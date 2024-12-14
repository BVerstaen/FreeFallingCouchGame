// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/CharacterIndicatorWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Characters/FreeFallCharacter.h"
#include "Kismet/GameplayStatics.h"

void UCharacterIndicatorWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCharacterIndicatorWidget::SetPlayerIndex(int index)
{
	PlayerIndex = index;
	ReceiveSetWidgetFromPlayerIndex(PlayerIndex);
}

void UCharacterIndicatorWidget::ShowCharacter(FVector2D CharacterScreenPos, FVector2D ViewportBoundsMin, FVector2D ViewportBoundsMax)
{
	FVector2D WidgetSize = GetDesiredSize();
	float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	FVector2D WidgetPosition;

	//Calculate WidgetPosition depending on WidgetDesiredSize, ViewportBounds and Character ScreenPos;
	float BoundsXMin = ViewportBoundsMin.X + (WidgetSize.X * ViewportScale) / 2.f;
	float BoundsXMax = ViewportBoundsMax.X - (WidgetSize.X * ViewportScale) / 2.f;
	float BoundsYMin = ViewportBoundsMin.Y + (WidgetSize.Y * ViewportScale) / 2.f;
	float BoundsYMax = ViewportBoundsMax.Y - (WidgetSize.Y * ViewportScale) / 2.f;
	
	WidgetPosition.X = FMath::Clamp(CharacterScreenPos.X, BoundsXMin, BoundsXMax);
	WidgetPosition.Y = FMath::Clamp(CharacterScreenPos.Y, BoundsYMin, BoundsYMax);

	SetPositionInViewport(WidgetPosition);
	
	//Calculate Cursor Angle depending on Character's ScreenPos and ViewportSize
	FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(GetWorld());
	float CursorDirX = CharacterScreenPos.X - (ViewportSize.X / 2.f);
	float CursorDirY = CharacterScreenPos.Y - (ViewportSize.Y / 2.f);

	float CursorAngle = FMath::RadiansToDegrees(FMath::Atan2(CursorDirY, CursorDirX));

	ReceiveShowOffScreenCharacterPos(CursorAngle);
}
