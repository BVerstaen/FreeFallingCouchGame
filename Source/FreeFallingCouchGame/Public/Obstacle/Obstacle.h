﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/GrabbableInterface.h"
#include "GameFramework/Actor.h"
#include "UI/BaseWarningActor.h"
#include "Interface/BounceableInterface.h"
#include "VFX/DepthAffectedObj.h"
#include "Obstacle.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacle : public AActor, public IGrabbableInterface, public IBounceableInterface
{
	GENERATED_BODY()

#pragma region Unreal Properties
	
public:
	// Sets default values for this actor's properties
	AObstacle();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
#pragma endregion

#pragma region Parameters

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> Mesh;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> Layer1Material;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> Layer2Material;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterial> Layer3Material;

	UPROPERTY(EditAnywhere)
	bool IsGrabbable = true;
	
	/*Sera modifiée par l'Obstacle Spawner*/
	UPROPERTY(EditAnywhere)
	float Speed;

	/*Sera modifiée par l'Obstacle Spawner*/
	UPROPERTY(EditAnywhere)
	FVector Direction;

	/*le poids de l'obstacle*/
	UPROPERTY(EditAnywhere)
	float ObjectMass;

	/*Multiplicateur individuelle pour chaque Obstacle*/
	UPROPERTY(EditAnywhere)
	float IndividualSpeedMultiplier = 1.0f;
	
#pragma endregion

#pragma region Sounds

public:
	UPROPERTY(EditAnywhere)
	TArray<FName> SoundsOnSpawn;

	UPROPERTY(EditAnywhere)
	FName SoundsOnCollision;

	UPROPERTY(EditAnywhere)
	float CollisionSoundCooldownTime = 1.0f;
	
	UPROPERTY()
	bool bCanPlayCollisionSound = true;

	UPROPERTY()
	FTimerHandle CollisionSoundCooldownTimerHandle;

	UFUNCTION()
	void ResetCollisionSoundCooldown();
	
#pragma endregion
	
	UFUNCTION(BlueprintCallable)
	void ResetLaunch();

	UFUNCTION()
	virtual bool CanBeGrabbed() override;

	UFUNCTION()
	virtual bool CanBeTaken() override;

	UFUNCTION()
	void OnHitDestroyWarning(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void SetupWarning(FVector ImpulseVector, FVector InDirection);
	
	UPROPERTY()
	ABaseWarningActor* LinkedWarningActor;

	UFUNCTION()
	void DebugRayTrace(TArray<FHitResult> RV_Hits, FVector const& EndLocation);
	
#pragma region Bounceable
public:
	
	virtual FVector GetVelocity() override;
	
	virtual float GetMass() override;
	
	virtual EBounceParameters GetBounceParameterType() override;
	
	virtual void AddBounceForce(FVector Velocity) override;
	
	virtual AFreeFallCharacter* CollidedWithPlayer() override;
	
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="Bounced"))
	void BounceSpecificBehaviour();
	
protected:
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBounceParameters> BounceType = EBounceParameters::Obstacle;
#pragma endregion 
};
