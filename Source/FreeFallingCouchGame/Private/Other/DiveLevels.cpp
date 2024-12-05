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

}

// Called when the game starts or when spawned
void ADiveLevels::BeginPlay()
{
	Super::BeginPlay();

	CameraActor = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));
	MapSettings = GetDefault<UMapSettings>();
}

// Called every frame
void ADiveLevels::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<TScriptInterface<IDiveLayersSensible>> ActorsToRemove;
	
	for (TScriptInterface<IDiveLayersSensible> SensibleActor : OverlappingSensibleActors)
	{
		if (SensibleActor == nullptr)
		{
			ActorsToRemove.Add(SensibleActor);
			continue;
		}
		if (SensibleActor->IsDiveForced())
		{
			ApplyDiveForce(SensibleActor);
		}
		if (SensibleActor->IsBoundedByLayer())
		{
			EDiveLayersID BoundingLayer = SensibleActor->GetBoundedLayer();
			float ZPos = SensibleActor->GetSelfActor()->GetActorLocation().Z;
			float UpZBound = 0;
			float DownZBound = 0;
			float ZVelocity = SensibleActor->GetSelfActor()->GetRootComponent()->ComponentVelocity.Z;
			if (BoundingLayer==EDiveLayersID::None)
			{
				UpZBound = GetDiveBoundZCoord(EDiveLayersID::Top, EDiveLayerBoundsID::Up);
				DownZBound = GetDiveBoundZCoord(EDiveLayersID::Bottom, EDiveLayerBoundsID::Down);
			}
			else
			{
				UpZBound = GetDiveBoundZCoord(BoundingLayer, EDiveLayerBoundsID::Up);
				DownZBound = GetDiveBoundZCoord(BoundingLayer, EDiveLayerBoundsID::Down);
			}
			float Force;
			FVector Direction = GetActorLocation() - CameraActor->GetActorLocation();
			Direction.Normalize();
			if (UpZBound - DiveLayerSlowingThreshold < ZPos)
			{
				Force = FMath::Clamp((UpZBound - ZPos) / DiveLayerSlowingThreshold,0, 1) * FMath::Abs(ZVelocity);
				GEngine->AddOnScreenDebugMessage(-1,DeltaTime,FColor::Emerald, TEXT("Force = " + FString::SanitizeFloat(Force)));
				SensibleActor->GetSelfActor()->GetRootComponent()->ComponentVelocity -= Direction * Force;
			}
			if (DownZBound + DiveLayerSlowingThreshold > ZPos)
			{
				Force = FMath::Clamp((ZPos - DownZBound) / DiveLayerSlowingThreshold,0, 1) * FMath::Abs(ZVelocity);
				GEngine->AddOnScreenDebugMessage(-1,DeltaTime,FColor::Emerald, TEXT("Force = " + FString::SanitizeFloat(Force)));
				SensibleActor->GetSelfActor()->GetRootComponent()->ComponentVelocity += Direction * Force;
			}
		}
	}
	
	for (TScriptInterface<IDiveLayersSensible> ActorToRemove : ActorsToRemove)
	{
		OverlappingSensibleActors.Remove(ActorToRemove);
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
		if (bDisableTopLayer) return EDiveLayersID::Middle;
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
		FVector Direction = (ZPosDifference < 0 ? -1 : 1 ) * (SensibleActor->GetSelfActor()->GetActorLocation() - CameraActor->GetActorLocation());
		Direction.Normalize();
		SensibleActor->ApplyDiveForce(Direction, DiveForcesStrength);
		//GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Red, "DiveForces");
	}
}


