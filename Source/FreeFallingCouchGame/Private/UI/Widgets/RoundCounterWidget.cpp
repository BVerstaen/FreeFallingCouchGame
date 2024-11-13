// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widgets/RoundCounterWidget.h"

void URoundCounterWidget::CallOnFinishCounter()
{
	OnFinishCounter.Broadcast();
}
