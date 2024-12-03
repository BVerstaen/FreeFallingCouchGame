// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DepthAffectedObj.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FREEFALLINGCOUCHGAME_API UDepthAffectedObj : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDepthAffectedObj();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor * Owner;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMeshComponent *MeshToChg;
	// temp,
	UPROPERTY()
	float P0 = 1970;
	UPROPERTY()
	float pMax = 2200;
	UPROPERTY()
	float pMin = 1800;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
