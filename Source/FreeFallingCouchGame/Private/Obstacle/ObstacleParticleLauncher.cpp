// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleParticleLauncher.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


// Sets default values
AObstacleParticleLauncher::AObstacleParticleLauncher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObstacleParticleLauncher::BeginPlay()
{
	Super::BeginPlay();

	
}

// Called every frame
void AObstacleParticleLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstacleParticleLauncher::SpawnParticle()
{
	//Spawn Particle
	EffectInstance = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleToSpawn.LoadSynchronous(), GetActorLocation());
	if(EffectInstance)
	{
		EffectInstance->Activate();
	}
	
	//Play lifetime timer
	GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &AObstacleParticleLauncher::LifeTimeEnd, LifeTime, false);
}

void AObstacleParticleLauncher::HitPlayer()
{
}

void AObstacleParticleLauncher::LifeTimeEnd()
{
	if(EffectInstance)
		EffectInstance->DestroyInstance();
	Destroy();
}

