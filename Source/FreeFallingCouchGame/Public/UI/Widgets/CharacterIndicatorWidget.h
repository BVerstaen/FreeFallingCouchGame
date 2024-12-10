// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterIndicatorWidget.generated.h"

class AFreeFallCharacter;
/**
 * 
 */
UCLASS(Abstract)
class FREEFALLINGCOUCHGAME_API UCharacterIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void NativeConstruct() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int PlayerIndex;

	void SetPlayerIndex(int index);

	UPROPERTY()
	TObjectPtr<APlayerController> MainCameraController;
	
	void ShowCharacter(FVector2D CharacterScreenPos, FVector2D ViewportSizeMin, FVector2D ViewportSizeMax);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Show OffScreen Character Indication"))
	void ReceiveShowOffScreenCharacterPos(float ArrowAngle);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Set Widget From PlayerIndex"))
	void ReceiveSetWidgetFromPlayerIndex(int Index);
};
