// Fill out your copyright notice in the Description page of Project Settings.


#include "VFX/DepthAffectedObj.h"
#include "Components/PrimitiveComponent.h"


// Sets default values for this component's properties
UDepthAffectedObj::UDepthAffectedObj()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UDepthAffectedObj::BeginPlay()
{
	Super::BeginPlay();
		Owner = GetOwner();
    	if(Owner->IsValidLowLevel()) {
    		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "Valid owner");
    		MeshToChg = Owner->FindComponentByClass<UMeshComponent>();
    		if(MeshToChg->IsValidLowLevel())
    		{
    		    //TODO Enable RenderDepthPass MeshToChg
    			MeshToChg->SetRenderInDepthPass(true);
    		} else {
    			PrimaryComponentTick.bCanEverTick = false;
    		}
    		
    	} else {
    		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Invalid owner");
    		PrimaryComponentTick.bCanEverTick = false;
    	}
}

// Called every frame
void UDepthAffectedObj::TickComponent(float DeltaTime, ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		
	float OwnerZ = Owner->GetActorLocation().Z;
	float x = 1- ((OwnerZ - pMin) / (pMax - pMin));
	
	x = FMath::Lerp(0.0f, 20.0f, x);
	MeshToChg->SetCustomDepthStencilValue(x);
}

