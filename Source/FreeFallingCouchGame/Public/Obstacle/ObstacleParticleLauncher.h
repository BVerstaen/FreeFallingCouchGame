﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Obstacle.h"
#include "ObstacleParticleLauncher.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleParticleLauncher : public AObstacle
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AObstacleParticleLauncher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*La particule à faire spawner*/
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UNiagaraSystem> ParticleToSpawn;

	/*la durée de vie de la particule (Détruira l'objet à la fin du timer)*/
	UPROPERTY(EditAnywhere)
	float LifeTime = -1.0f;
	
	UPROPERTY()
	FTimerHandle LifeTimerHandle;

protected:
	UNiagaraComponent* EffectInstance; 
	
public:
	UFUNCTION()
	void SpawnParticle();
	
	UFUNCTION()
	void HitPlayer();
	
	UFUNCTION()
	void LifeTimeEnd();
};
