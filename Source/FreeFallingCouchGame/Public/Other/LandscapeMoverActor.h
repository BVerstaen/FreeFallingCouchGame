// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "LandscapeMoverActor.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API ALandscapeMoverActor : public AActor
{
	GENERATED_BODY()

#pragma region Unreal Default

public:
	// Sets default values for this actor's properties
	ALandscapeMoverActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma endregion 

public:
	UFUNCTION(BlueprintCallable)
	void Restart();

	UFUNCTION(BlueprintCallable)
	void Pause();

	UFUNCTION()
	UCameraComponent* GetActiveCamera();
	
#pragma region Properties
	/*La courbe du landscape*/
	UPROPERTY(EditAnywhere, Category= "Landscape")
	TSoftObjectPtr<UCurveFloat> LandscapeCurve;
	UPROPERTY()
	TObjectPtr<UCurveFloat> CurrentLandscapeCurve;
	
	/*Le landscape qui sera affacté*/
	UPROPERTY(EditAnywhere, Category= "Landscape")
	TObjectPtr<AActor> Landscape;

	/*Les acteurs (maison, chateau) à mettre en enfant du landscape*/
	UPROPERTY(EditAnywhere, Category= "Landscape")
	TArray<TObjectPtr<AActor>> LandscapeChild;

	/*La "position minimum" a.k.a la + proche de la caméra*/
	UPROPERTY(EditAnywhere, Category= "Lerp")
	FTransform MinimumTransform;

	/*La "position maximum" a.k.a la + loin de la caméra*/
	UPROPERTY(EditAnywhere, Category= "Lerp")
	FTransform MaximumTransform;

	UPROPERTY(BlueprintReadWrite, Category = "Lerp")
	float Timer;
	
	UPROPERTY(BlueprintReadWrite, Category = "Lerp")
	float MaxTimer;

	bool bIsLerping = false;
	
	UPROPERTY()
	UCameraComponent* FishEyeCameraComponent;
	
	float FishEyePostProcessDefaultIntensity;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> PostProcessMaterial;

#pragma endregion
};
