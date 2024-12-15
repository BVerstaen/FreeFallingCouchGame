// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "BaseWarningActor.generated.h"

UENUM(BlueprintType)
enum class EWarningType  : uint8
{
	Base,
	Vertical,
};

UCLASS()
class FREEFALLINGCOUCHGAME_API ABaseWarningActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWarningActor();
	
	// Functions linked to owner
	UFUNCTION(BlueprintCallable)
	FHitResult GetHitResult() const {return DataHit;}
	UFUNCTION(BlueprintCallable)
	void SetHitResult(const FHitResult& InHitResult) {DataHit= InHitResult;}
	UFUNCTION(BlueprintCallable)
	void SetDirection(const FVector InDir) {DirectionObject = InDir;}
	UFUNCTION(BlueprintCallable)
	void SetLinkedObstacle(AActor* NewOwner) {OwnerObstacle = NewOwner;}
	
	// Functions linked to itself
	UFUNCTION(BlueprintCallable)
	void CheckDistanceColor();
	UFUNCTION()
	void SetupWidgets();
	UFUNCTION()
	void OverwriteWidgets() const;
	UFUNCTION()
	void FaceCamera();
	
protected:
	UPROPERTY()
	TSubclassOf<UUserWidget> CachedWidgetClass;
	// Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent *Arrow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* RootBody;
	// Ref
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* OwnerObstacle;
	//Parameters gotten from owner
	UPROPERTY()
	EWarningType WarningType = EWarningType::Base;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	FVector DirectionObject;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult DataHit;
	UPROPERTY()
	APlayerCameraManager* CameraManagerRef;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintImplementableEvent)
	void ObjectFarEvent(EWarningType WarningTypeSent);
	UFUNCTION(BlueprintImplementableEvent)
	void ObjectCloserEvent(EWarningType WarningTypeSent);
	UFUNCTION(BlueprintImplementableEvent)
	void ObjectVeryNearEvent(EWarningType WarningTypeSent);
};