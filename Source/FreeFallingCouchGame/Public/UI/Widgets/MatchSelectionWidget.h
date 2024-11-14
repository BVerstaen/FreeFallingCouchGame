#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/UniformGridPanel.h"
#include "FreeFallingCouchGame/Public/Match/MatchParameters.h"
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
	UFUNCTION(BlueprintCallable, Category = "MatchSelectionWidget")
	void ReceiveData(int inMaxRounds = 3,float inRoundTimer = 55.0f, float inEventDelay = 45.0f, FString inEraChosen = "Default",
		 EMatchTypes InMatchType = Classic, ETrackingRewardCategory InTracker = LongestTimeWithParachute);
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Match Data")
	UMatchParameters *PlayerCustomData;

	UPROPERTY(EditAnywhere, Category = "Blueprint Data")
	TSubclassOf<UUserWidget> ItemWidgetClass = nullptr;
}
