// Fill out your copyright notice in the Description page of Project Settings.


#include "Other/DiveLevels.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Other/DiveLayersID.h"
#include "Interface/DiveLayersSensible.h"
#include "Settings/MapSettings.h"


// Sets default values
ADiveLevels::ADiveLevels()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MapSettings = GetDefault<UMapSettings>();
}

// Called when the game starts or when spawned
void ADiveLevels::BeginPlay()
{
	Super::BeginPlay();

	CameraActor = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));
}

// Called every frame
void ADiveLevels::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (TScriptInterface<IDiveLayersSensible> SensibleActor : OverlappingSensibleActors)
	{
		if (SensibleActor == nullptr)
		{
			OverlappingSensibleActors.Remove(SensibleActor);
			continue;
		}
		/*
		if (!IsValid(Cast<AActor>(SensibleActor)))
			GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::White, TEXT("Actor not valid "));
		*/
		GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::White, TEXT("List Contains " + SensibleActor->GetSelfActor()->GetName()));
		if (SensibleActor->IsDiveForced())
		{
			ApplyDiveForce(SensibleActor);
			GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::White, TEXT(" Applying DiveForce to " + SensibleActor->GetSelfActor()->GetName()));
		}
	}
}

float ADiveLevels::GetDiveBoundZCoord(EDiveLayersID Layer, EDiveLayerBoundsID Bound)
{
	float BoundMiddleCoordZ =
		Layer==EDiveLayersID::Top ? DiveLevelSize.Z * 2 + 1 :
		Layer==EDiveLayersID::Bottom ? DiveLevelSize.Z * -2 - 1 :
		0;

	BoundMiddleCoordZ+=
		Bound==EDiveLayerBoundsID::Up ? DiveLevelSize.Z :
		Bound==EDiveLayerBoundsID::Down ? -DiveLevelSize.Z :
		0;

	return GetActorLocation().Z + BoundMiddleCoordZ;
}

float ADiveLevels::GetDiveSize()
{
	return DiveLevelSize.Z;
}

EDiveLayersID ADiveLevels::GetDiveLayersFromCoord(float PlayerCoordZ)
{
	if (PlayerCoordZ > GetDiveBoundZCoord(EDiveLayersID::Top, EDiveLayerBoundsID::Down))
	{
		return EDiveLayersID::Top;
	}
	if (PlayerCoordZ < GetDiveBoundZCoord(EDiveLayersID::Bottom,  EDiveLayerBoundsID::Up))
	{
		return EDiveLayersID::Bottom;
	}
	return EDiveLayersID::Middle;
}

void ADiveLevels::ApplyDiveForce(TScriptInterface<IDiveLayersSensible> SensibleActor)
{
	if (SensibleActor == nullptr) return;

	float ZPosition = SensibleActor->GetSelfActor()->GetActorLocation().Z;
	EDiveLayersID CurrentLayer = GetDiveLayersFromCoord(ZPosition);
	float ZPosDifference = ZPosition - GetDiveBoundZCoord(CurrentLayer, EDiveLayerBoundsID::Middle);
	if (FMath::Abs(ZPosDifference) > MapSettings->DiveLayerThreshold)
	{
		FVector Direction = (ZPosDifference < 0 ? -1 : 1 ) * (GetActorLocation() - CameraActor->GetActorLocation());
		Direction.Normalize();
		SensibleActor->ApplyDiveForce(Direction, DiveForcesStrength);
		GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Red, "DiveForces");
	}
}


