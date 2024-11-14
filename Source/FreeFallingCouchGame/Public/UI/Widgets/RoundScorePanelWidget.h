// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundScorePanelWidget.generated.h"

enum ETrackingRewardCategory;
/**
 * 
 */
UCLASS()
class FREEFALLINGCOUCHGAME_API URoundScorePanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "C++ Functions")
	void SetPlayerProfile(int PlayerID, int PlayerScore, int MaxScore);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "C++ Functions")
	void ShowRoundScoreAnimation();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishShowAnimation);
	FOnFinishShowAnimation OnFinishShow;

	UFUNCTION(BlueprintCallable)
	void CallOnFinishShow();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "C++ Functions")
	void HideRoundScoreAnimation();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishHideAnimation);
	FOnFinishHideAnimation OnFinishHide;

	UFUNCTION(BlueprintCallable)
	void CallOnFinishHide();
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "C++ Functions")
	void AddScoreToRound(int PlayerID,int Score);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "C++ Functions")
	void AddScoreReward(int PlayerID,int Score, ETrackingRewardCategory Category, float DelayDisplayedOnScreen);
};
