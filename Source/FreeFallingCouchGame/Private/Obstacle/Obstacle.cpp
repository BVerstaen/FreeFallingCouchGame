﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Obstacle/Obstacle.h"
#include "Audio/SoundSubsystem.h"


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
	Mesh->bRenderCustomDepth = true;
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
	Super::BeginPlay();

	UDepthAffectedObj* DepthComponent = NewObject<UDepthAffectedObj>(this, TEXT("DepthComponent"));
	if (IsValid(DepthComponent))
	{
		AddInstanceComponent(DepthComponent);
		DepthComponent->RegisterComponent();
	}

	FVector refDirection = Direction;
	Direction.Normalize();
	FVector ImpulseVector = Direction * Speed * IndividualSpeedMultiplier;
	if(Mesh != nullptr)
	{
		Mesh->AddImpulse(ImpulseVector, NAME_None, true);
		// Shoot raytrace to set warning
		SetupWarning(ImpulseVector, refDirection);
	}
}
#pragma region Warnings
void AObstacle::DebugRayTrace(TArray<FHitResult> RV_Hits, FVector const& EndLocation)
{
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
}

void AObstacle::SetupWarning(FVector ImpulseVector, FVector InDirection)
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
	//DrawDebugLine(GetWorld(), GetActorLocation(), EndLocation, FColor::Red, false, 2.0f);
	if(!RV_Hits.IsEmpty())
	{
		for (const FHitResult& Hit : RV_Hits)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit Actor: %s, Channel: %d"), *Hit.GetActor()->GetName(), (int32)Hit.GetComponent()->GetCollisionObjectType());
		}
		
		//DebugRayTrace(RV_Hits, EndLocation);
		// Initiate Warning actor
		TSubclassOf<ABaseWarningActor> ThingToSpawn = ABaseWarningActor::StaticClass();
		
		FTransform SpawnTransform = GetTransform();
		FVector OriginPoint = RV_Hits.GetData()[0].Location;
		SpawnTransform.SetLocation(OriginPoint);
		LinkedWarningActor = GetWorld()->SpawnActorDeferred<ABaseWarningActor>(
			ThingToSpawn,
			SpawnTransform
			);
		//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Cyan, FString::Printf(
		//TEXT("Vector of X:%f Y:%f Z:%f value "),InDirection.X,InDirection.Y,InDirection.Z));
		LinkedWarningActor->SetDirection(InDirection);
		LinkedWarningActor->SetHitResult(RV_Hits.GetData()[0]);
		LinkedWarningActor->SetLinkedObstacle(this);
		LinkedWarningActor->FinishSpawning(SpawnTransform);
		Mesh->OnComponentHit.AddDynamic(this, &AObstacle::OnHitDestroyWarning);
	} 
}

void AObstacle::OnHitDestroyWarning(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(IsValid(LinkedWarningActor))
	{
		LinkedWarningActor->ForceKill();
	}
	LinkedWarningActor = nullptr;
}
#pragma endregion

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
	return ObjectMass;
}

EBounceParameters AObstacle::GetBounceParameterType()
{
	return BounceType;
}

void AObstacle::AddBounceForce(FVector Velocity)
{
	Mesh->AddForce(Velocity);
	
}

AFreeFallCharacter* AObstacle::CollidedWithPlayer()
{
	//Play collision sound
	if(bCanPlayCollisionSound)
	{
		USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
		SoundSubsystem->PlaySound(SoundsOnCollision, this, true);

		bCanPlayCollisionSound = false;
		GetWorldTimerManager().SetTimer(CollisionSoundCooldownTimerHandle, this, &AObstacle::ResetCollisionSoundCooldown,
			CollisionSoundCooldownTime, false, CollisionSoundCooldownTime);
	}

	BounceSpecificBehaviour();
	
	return nullptr;
}

void AObstacle::ResetCollisionSoundCooldown() { bCanPlayCollisionSound = true; }

#pragma endregion
