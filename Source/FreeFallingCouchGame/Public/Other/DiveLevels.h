// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DiveLevels.generated.h"

class UMapSettings;
class IDiveLayersSensible;
enum class EDiveLayerBoundsID : uint8;
enum class EDiveLayersID : uint8;

UCLASS()
class FREEFALLINGCOUCHGAME_API ADiveLevels : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ADiveLevels();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	float GetDiveBoundZCoord(EDiveLayersID Layer, EDiveLayerBoundsID Bound);

	float GetDiveSize();

	EDiveLayersID GetDiveLayersFromCoord(float PlayerCoordZ);

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FVector3f DiveLevelSize;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TScriptInterface<IDiveLayersSensible>> OverlappingSensibleActors;

	UPROPERTY()
	const UMapSettings* MapSettings;

private:
	void ApplyDiveForce(TScriptInterface<IDiveLayersSensible> SensibleActor);

	//Strength of Dive Forces
	UPROPERTY(EditAnywhere)
	float DiveForcesStrength;

	//Tweaking GP, Threshold à partir duquel le player est ralentit jusqu'à l'arrêt;
	UPROPERTY(EditAnywhere)
	float DiveLayerSlowingThreshold = 10.f;

	UPROPERTY()
	TObjectPtr<ACameraActor> CameraActor;
};
