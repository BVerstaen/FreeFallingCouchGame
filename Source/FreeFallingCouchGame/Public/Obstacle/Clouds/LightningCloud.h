// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LightningCloud.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API ALightningCloud : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALightningCloud();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetupLightningCloud(float m_TimeBeforeLightning, float m_LightningRadius, TObjectPtr<ACameraActor> Camera);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStruckLightning, ALightningCloud*, LightningCloudActor);
	FStruckLightning OnStruckLightning;

	UFUNCTION(BlueprintImplementableEvent)
	void PlaySound(const FString& SoundName);

	UPROPERTY(EditAnywhere)
	float TimeBeforeDestruction;

	UFUNCTION(BlueprintImplementableEvent)
	void StruckLightning();

	UPROPERTY(BlueprintReadOnly)
	float LightningRadius = 0.f;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDirectionDependsOnCamera;

	UPROPERTY(BlueprintReadOnly)
	FRotator LightningRotation;
	
	FVector ShootDirection;
	
private:
	float TimeBeforeLightning = 100.f;

	float LightningClock = 0.f;

	UPROPERTY()
	TObjectPtr<ACameraActor> CameraActor;


	bool bHasStruckLightning = false;

	void KillPlayerInsideLightning();

	UPROPERTY(EditAnywhere)
	float LightningLength = 100.f;
};
