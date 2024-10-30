// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/MatchSelectionWidget.h"

void UMatchSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Bind delegates and default appearance
}
// Called every time the widget is compiled / property changed
void UMatchSelectionWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
/*
	if(Label)
		Label->SetText(LabelText);

	if(Grid && ItemWidgetClass)
	{
		Grid->ClearChildren();
		for(int32 x = 0; x < Columns; ++x)
		{
			UUserWidget* Widget = CreateWidget<UUserWidget>
			(GetWorld(), ItemWidgetClass);
			if(Widget)
			{
				UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(
					Widget);
				GridSlot->SetColumn(Columns);
				GridSlot->SetRow(1);
			}
		}
	}
	*/
}

void UMatchSelectionWidget::TestCallOnPressed(FString NameElementPressed)
{
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(TEXT("Current Round: %s"), *NameElementPressed));

}

void UMatchSelectionWidget::ReceiveData(int inMaxRounds,float inRoundTimer, float inEventDelay, FString inEraChosen,
		 EMatchTypes InMatchType, ETrackingRewardCategory InTracker)
{
	PlayerCustomData->setMatchParameters(
		inMaxRounds,
		inRoundTimer,
		inEventDelay,
		inEraChosen,
		InMatchType,
		InTracker);
}
