﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Other/LandscapeMoverActor.h"

#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ALandscapeMoverActor::ALandscapeMoverActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALandscapeMoverActor::BeginPlay()
{
	Super::BeginPlay();

	//Add childs as child of landscape

	if(!Landscape) return;
	
	for(AActor* child : LandscapeChild)
	{
		FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);
		child->AttachToActor(Landscape, AttachmentRules);
	}
	
	//Set fish eye post process
	FishEyeCameraComponent = GetActiveCamera();
	if (FishEyeCameraComponent)
	{
		if(FishEyeCameraComponent->PostProcessSettings.WeightedBlendables.Array.Num() <= 0)
		{
			GEngine->AddOnScreenDebugMessage(-1,30.f, FColor::Red, "No FishEye shader in camera (also set weight to 0.01f)");
		}
	}
}

// Called every frame
void ALandscapeMoverActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(!bIsLerping) return;
	
	float Progression = Timer / MaxTimer;

	//GEngine->AddOnScreenDebugMessage(-1,DeltaTime,FColor::Red,"Progression : " + FString::SanitizeFloat(Progression));

	FVector newPos = UKismetMathLibrary::VLerp(MinimumTransform.GetLocation(), MaximumTransform.GetLocation(), Progression);
	FRotator newRotator = UKismetMathLibrary::RLerp(MinimumTransform.GetRotation().Rotator(), MaximumTransform.GetRotation().Rotator(), Progression, true);
	FVector newScale = UKismetMathLibrary::VLerp(MinimumTransform.GetScale3D(), MaximumTransform.GetScale3D(), Progression);

	//Set Landscape transform
	Landscape->SetActorLocation(newPos);
	Landscape->SetActorRotation(newRotator);
	Landscape->SetActorScale3D(newScale);

	//Set Post process intensity
	if(FishEyeCameraComponent && FishEyeCameraComponent->PostProcessSettings.WeightedBlendables.Array.Num() > 0)
	{
		if(!FMath::IsNaN(1-Progression))
			FishEyeCameraComponent->PostProcessSettings.WeightedBlendables.Array[0].Weight = (1-Progression);
	}
	
	if(Progression < 0.0f)
		Pause();
}

void ALandscapeMoverActor::Restart()
{
	Landscape->SetActorTransform(MaximumTransform);
	MaxTimer = Timer;

	bIsLerping = true;
}

void ALandscapeMoverActor::Pause()
{
	bIsLerping = false;
}

UCameraComponent* ALandscapeMoverActor::GetActiveCamera()
{
	//Get player controller
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		//Get camera
		AActor* ViewTarget = PlayerController->GetViewTarget();
		if (ViewTarget)
		{
			//Get camera component
			UCameraComponent* CameraComponent = ViewTarget->FindComponentByClass<UCameraComponent>();
			return CameraComponent;
		}
	}

	return nullptr;
}