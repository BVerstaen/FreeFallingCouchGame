// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Obstacle.h"


// Sets default values
AObstacle::AObstacle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//Create Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	RootComponent = Mesh;
	
	//Initialize parameters
	Speed = 0.0f;
	Direction = FVector::ZeroVector;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();
	

	Direction.Normalize();
	FVector ImpulseVector = Direction * Speed;
	if(Mesh != nullptr)
	{
		Mesh->AddImpulse(ImpulseVector, NAME_None, true);
		// Shoot raytrace to set warning
		SetupWarning(ImpulseVector);
	}
}


void AObstacle::SetupWarning(FVector ImpulseVector)
{
	//Setup parameters
	TArray<FHitResult> RV_Hits;
	//FHitResult RV_Hit(ForceInit);
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	RV_TraceParams.bTraceComplex = true;
	RV_TraceParams.bReturnPhysicalMaterial = false;

	FVector EndLocation = GetActorLocation() + (ImpulseVector.GetSafeNormal() * 15000.0f);
	// Shoot linetrace
	GetWorld()->LineTraceMultiByChannel(
		RV_Hits,
		GetActorLocation(),
		EndLocation,
		ECC_GameTraceChannel2,
		RV_TraceParams
		);
	DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Red, false, 2.0f);
	if(!RV_Hits.IsEmpty())
	{
		for (const FHitResult& Hit : RV_Hits)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s, Channel: %d"), *Hit.GetActor()->GetName(), (int32)Hit.GetComponent()->GetCollisionObjectType());
		}
		
		if (RV_Hits.GetData()[0].IsValidBlockingHit())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Hit detected"));
			DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Green, false, 5.0f, 0, 5.0f);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No hit detected"));
			DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Red, false, 5.0f, 0, 5.0f);
		}
		// Initiate Warning actor
		TSubclassOf<ABaseWarningActor> ThingToSpawn = ABaseWarningActor::StaticClass();
		
		FTransform SpawnTransform = GetTransform();
		FVector OriginPoint = RV_Hits.GetData()[0].Location;
		SpawnTransform.SetLocation(OriginPoint);
		ABaseWarningActor* LinkedWarningActor = GetWorld()->SpawnActorDeferred<ABaseWarningActor>(
			ThingToSpawn,
			SpawnTransform
			);
		LinkedWarningActor->SetHitResult(RV_Hits.GetData()[0]);
		LinkedWarningActor->SetLinkedObstacle(this);
		LinkedWarningActor->FinishSpawning(SpawnTransform);
	} 
}

void AObstacle::ResetLaunch()
{
	Mesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector, false);
	Direction.Normalize();
	FVector ImpulseVector = Direction * Speed;
	Mesh->AddImpulse(ImpulseVector, NAME_None, true);
}

bool AObstacle::CanBeGrabbed()
{
	return IsGrabbable;
}

bool AObstacle::CanBeTaken()
{
	return false;
}

#pragma region Bounceable

FVector AObstacle::GetVelocity()
{
	return Mesh->GetPhysicsLinearVelocity();
}

float AObstacle::GetMass()
{
	return Mesh->GetMass();
}

EBounceParameters AObstacle::GetBounceParameterType()
{
	return Obstacle;
}

void AObstacle::AddBounceForce(FVector Velocity)
{
	Mesh->AddForce(Velocity);
}

AFreeFallCharacter* AObstacle::CollidedWithPlayer()
{
	return nullptr;
}

#pragma endregion
