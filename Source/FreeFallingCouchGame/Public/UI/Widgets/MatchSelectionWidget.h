// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/PlayerMatchData.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "MatchSelectionWidget.generated.h"

/**
 * 
 */

// Class is abstract since we want to make instances of the bp created with this instead
UCLASS(Abstract)
class UMatchSelectionWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	//Native construct is more useful than regular cpp constructor for widgets
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

	UFUNCTION(BlueprintCallable, Category = "MatchSelectionWidget")
	void TestCallOnPressed(FString NameElementPressed);
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Data")
	UPlayerMatchData *PlayerCustomData;

	UPROPERTY(EditAnywhere, Category = "Blueprint Data")
	TSubclassOf<UUserWidget> ItemWidgetClass = nullptr;
	UPROPERTY(EditAnywhere, Category = "Blueprint Data", meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> Grid;
};
