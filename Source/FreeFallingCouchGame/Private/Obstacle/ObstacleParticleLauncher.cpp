// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/ObstacleParticleLauncher.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Characters/FreeFallCharacter.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


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

	GetWorldTimerManager().SetTimer(LaunchProjectileTimerHandle, this, &AObstacleParticleLauncher::SpawnParticle, LaunchProjectileTimer, false);
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
		EffectInstance->SetVariableObject("BPCallBackHandler", this);
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

void AObstacleParticleLauncher::ReceiveParticleData_Implementation(const TArray<FBasicParticleData>& Data,
	UNiagaraSystem* NiagaraSystem, const FVector& SimulationPositionOffset)
{
	INiagaraParticleCallbackHandler::ReceiveParticleData_Implementation(Data, NiagaraSystem, SimulationPositionOffset);

	for(FBasicParticleData Particle : Data)
	{
		AFreeFallCharacter* FoundCharacter = GetHitPlayer(Particle.Position, Particle.Velocity);
		if(!FoundCharacter) return;

		if(AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
		{
			GameMode->CallArenaActorOnCharacterDestroyed(FoundCharacter);
			FoundCharacter->DestroyPlayer();
		}
	}
}

AFreeFallCharacter* AObstacleParticleLauncher::GetHitPlayer(FVector StartPosition, FVector Velocity)
{
	
	FVector EndPosition = StartPosition - Velocity.GetSafeNormal() * CheckForwardDistance;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	TArray<AActor*> ignoreActors;
	FHitResult HitResult;
	
	bool bHasHit = UKismetSystemLibrary::LineTraceSingleForObjects(
														GetWorld(),
														StartPosition,
														EndPosition,
														traceObjectTypes,
														false,
														ignoreActors,
														EDrawDebugTrace::Persistent,
														HitResult,
														true);
	
	if (bHasHit)
	{
		if(AFreeFallCharacter* OtherActor =  Cast<AFreeFallCharacter>(HitResult.GetActor()))
		{
			return OtherActor;
		}
	}
	return nullptr;
}

