// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Widgets/MatchSelectionWidget.h"

#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Match/GameDataInstanceSubsystem.h"

void UMatchSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Bind delegates and default appearance
}
// Called every time the widget is compiled / property changed
void UMatchSelectionWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UMatchSelectionWidget::TestCallOnPressed(FString NameElementPressed)
{
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(TEXT("Current Round: %s"), *NameElementPressed));

}

void UMatchSelectionWidget::ReceiveData(int inMaxRounds,float inRoundTimer, float inEventDelay, FString inEraChosen,
		 EMatchTypes InMatchType, ETrackingRewardCategory InTracker)
	{
	//AFreeFallGameMode* MyMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	//if(MyMode->IsValidLowLevel())

	UGameDataInstanceSubsystem* GameDataSubsystem = GetGameInstance()->GetSubsystem<UGameDataInstanceSubsystem>();
	if(GameDataSubsystem->IsValidLowLevel())
	{
		GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(TEXT("Valid Subsystem")));
		GameDataSubsystem->CreateMatchParameters();
		GameDataSubsystem->GetMatchParameters()->setMatchParameters (
	   inMaxRounds,
	   inRoundTimer,
	   inEventDelay,
	   inEraChosen,
	   InMatchType,
	   InTracker);
	}
}
