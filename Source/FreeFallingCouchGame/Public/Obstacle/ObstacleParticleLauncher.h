// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Obstacle.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceExport.h"
#include "ObstacleParticleLauncher.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AObstacleParticleLauncher : public AObstacle, public INiagaraParticleCallbackHandler
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
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> EffectInstance; 
	
public:
	UFUNCTION()
	void SpawnParticle();
	
	UFUNCTION()
	void HitPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void LifeTimeEnd();

	UFUNCTION()
	virtual void ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data, UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset) override;

	UFUNCTION()
	AFreeFallCharacter* GetHitPlayer(FVector StartPosition, FVector Velocity);

	UPROPERTY(EditAnywhere)
	float CheckForwardDistance;
};
