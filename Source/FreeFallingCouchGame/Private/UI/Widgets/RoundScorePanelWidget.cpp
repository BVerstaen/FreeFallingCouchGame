// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/RoundScorePanelWidget.h"

void URoundScorePanelWidget::CallOnFinishShow()
{
	OnFinishShow.Broadcast();
}

void URoundScorePanelWidget::CallOnFinishHide()
{
	OnFinishHide.Broadcast();
}
