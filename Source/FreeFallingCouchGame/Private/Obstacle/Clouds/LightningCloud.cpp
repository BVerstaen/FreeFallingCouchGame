// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Clouds/LightningCloud.h"

#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacter.h"
#include "GameMode/FreeFallGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ALightningCloud::ALightningCloud()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALightningCloud::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightningCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LightningClock += DeltaTime;
	if (LightningClock >= TimeBeforeLightning && !bHasStruckLightning)
	{
		bHasStruckLightning = true;
		StruckLightning();
		KillPlayerInsideLightning();
		OnStruckLightning.Broadcast(this);
	}
	if (LightningClock >= TimeBeforeLightning + TimeBeforeDestruction && bHasStruckLightning)
	{
		Destroy();
	}
}

void ALightningCloud::SetupLightningCloud(float m_TimeBeforeLightning, float m_LightningRadius, TObjectPtr<ACameraActor> Camera)
{
	TimeBeforeLightning = m_TimeBeforeLightning;
	LightningRadius = m_LightningRadius;
	CameraActor = Camera;

	ShootDirection = bDirectionDependsOnCamera? GetActorLocation() - CameraActor->GetActorLocation() : -GetActorUpVector();

	LightningRotation = ShootDirection.Rotation();
	
	ShootDirection.Normalize();

	
}

void ALightningCloud::KillPlayerInsideLightning()
{
	FVector StartPoint = GetActorLocation();
	FVector EndPoint = GetActorLocation() + ShootDirection * LightningLength;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	TArray<AActor*> ignoreActors;
	TArray<FHitResult> HitResults;
    
	bool hasKilledPlayer = UKismetSystemLibrary::SphereTraceMultiForObjects(
														GetWorld(),
														StartPoint,
														EndPoint,
														LightningRadius,
														traceObjectTypes,
														false,
														ignoreActors,
														EDrawDebugTrace::ForOneFrame,
														HitResults,
														true);

	if (!hasKilledPlayer) return;

	for (FHitResult HitResult : HitResults)
	{
		TObjectPtr<AFreeFallCharacter> Character = Cast<AFreeFallCharacter>(HitResult.GetActor());
		if (Character)
		{
			if (AFreeFallGameMode* GameMode = Cast<AFreeFallGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
			{
				GameMode->CallArenaActorOnCharacterDestroyed(Character);
				Character->DestroyPlayer(ETypeDeath::Classic);
			}
		}
	}
}

