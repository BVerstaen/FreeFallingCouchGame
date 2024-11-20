// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleParticleLauncher.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Characters/FreeFallCharacter.h"


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

	SpawnParticle();
}

// Called every frame
void AObstacleParticleLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AObstacleParticleLauncher::SpawnParticle()
{
	//Spawn Particle
	EffectInstance = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ParticleToSpawn.LoadSynchronous(), GetActorLocation(), Direction.ToOrientationRotator());
	if(EffectInstance)
	{
		EffectInstance->Activate();
		EffectInstance->OnComponentHit.AddDynamic(this, &AObstacleParticleLauncher::HitPlayer);
		InitParticleBlueprint();
	}
	
	//Play lifetime timer
	GetWorldTimerManager().SetTimer(LifeTimerHandle, this, &AObstacleParticleLauncher::LifeTimeEnd, LifeTime, false);
}

void AObstacleParticleLauncher::HitPlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Hit");
	if(AFreeFallCharacter* HitCharacter = Cast<AFreeFallCharacter>(OtherActor))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Player Hit");
	}
}

void AObstacleParticleLauncher::LifeTimeEnd()
{
	if(EffectInstance)
		EffectInstance->DestroyInstance();
	Destroy();
}

