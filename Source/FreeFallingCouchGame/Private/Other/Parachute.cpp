// Fill out your copyright notice in the Description page of Project Settings.


#include "Other/Parachute.h"

#include "Characters/FreeFallCharacter.h"


// Sets default values
AParachute::AParachute()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(false);
}

// Called when the game starts or when spawned
void AParachute::BeginPlay()
{
	Super::BeginPlay();
	StartingLocation = GetActorLocation();
}

// Called every frame
void AParachute::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AParachute::CanBeTaken()
{
	return true;
}

bool AParachute::CanBeGrabbed()
{
	return false;
}

void AParachute::Use(AFreeFallCharacter* Character)
{
	OriginScale = GetActorRelativeScale3D();
	if(!Character) return;
	
	//Give reference & attach self to Character
	Character->Parachute = this;
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,EAttachmentRule::KeepWorld, true);
	AttachToComponent(Character->GetParachuteAttachPoint(), AttachmentRules);
	
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Character->OnWasEliminated.AddDynamic(this, &AParachute::GiveToMurderer);
	
	OnParachuteGrabbed.Broadcast(Character);

	SetActorRelativeScale3D(OriginScale);
}

void AParachute::StealParachute(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NextOwner)
{
	if(!PreviousOwner || !NextOwner) return;
	
	//Change Reference between Characters
	PreviousOwner->Parachute = nullptr;
	NextOwner->Parachute = this;

	//Detach from old actor and attach to new one
	FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	DetachFromActor(DetachmentRules);
	FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget,EAttachmentRule::KeepWorld, true);
	AttachToComponent(NextOwner->GetParachuteAttachPoint(), AttachmentRules);

	OnParachuteStolen.Broadcast(PreviousOwner, NextOwner);

	SetActorRelativeScale3D(OriginScale);
}

AParachute* AParachute::DropParachute(AFreeFallCharacter* PreviousOwner)
{
	if(!PreviousOwner) return nullptr;

	PreviousOwner->Parachute = nullptr;
	
	//Detach from old actor and attach to new one
	FDetachmentTransformRules DetachmentRules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	DetachFromActor(DetachmentRules);

	SetActorLocation(StartingLocation);
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

	OnParachuteDropped.Broadcast(PreviousOwner);

	return this;
}

void AParachute::RecenterParachute() const
{
	Mesh->SetSimulatePhysics(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);

	FVector Direction = (StartingLocation - GetActorLocation()).GetSafeNormal();
	Mesh->AddImpulse(Direction * LaunchForce);
}

void AParachute::GiveToMurderer(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NextOwner)
{
	//Drop then give parachute to murderer
	DropParachute(PreviousOwner);
	Use(NextOwner);
}

