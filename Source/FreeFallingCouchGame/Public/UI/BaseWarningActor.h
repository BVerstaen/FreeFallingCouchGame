// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "BaseWarningActor.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API ABaseWarningActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWarningActor();
	UFUNCTION(BlueprintCallable)
	FHitResult GetHitResult() const {return DataHit;}
	UFUNCTION(BlueprintCallable)
	void SetHitResult(const FHitResult& InHitResult) {DataHit= InHitResult;}

	UFUNCTION(BlueprintCallable)
	void SetLinkedObstacle(AActor* NewOwner) {OwnerObstacle = NewOwner;}

	UFUNCTION(BlueprintCallable)
	void CheckDistanceColor();

	UFUNCTION()
	void SetupWidgets();

	void SetupWidget(UWidgetComponent* RefObject, FName inName, USceneComponent* root,
	FVector2D InSize2D, const FVector& InScale, const FVector& InPositionRelative, const TCHAR* InPath);
	UFUNCTION()
	void FaceCamera();
	UFUNCTION()
	void SetRotationBeginning();
protected:
	// Components
	UPROPERTY(EditAnywhere)
	UWidgetComponent *Arrow;
	//UPROPERTY(EditAnywhere) UWidgetComponent *Body;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite) USceneComponent* RootArrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootBody;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult DataHit;
	UPROPERTY()
	APlayerCameraManager* CameraManagerRef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OwnerObstacle;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};