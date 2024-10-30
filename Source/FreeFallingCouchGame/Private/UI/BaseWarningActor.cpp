// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BaseWarningActor.h"

#include "MovieSceneSequenceID.h"
#include "UnrealWidgetFwd.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseWarningActor::ABaseWarningActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootArrow = CreateDefaultSubobject<USceneComponent>(TEXT("RootArrow"));
	SetupWidgets();
}

void ABaseWarningActor::SetupWidgets()
{
	Arrow = CreateDefaultSubobject<UWidgetComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootArrow);
	Arrow->SetWidgetSpace(EWidgetSpace::World);
	Arrow->SetDrawSize(FVector2D(500.0f, 500.0f));
	Arrow->SetRelativeScale3D(FVector(0.24f, 0.24f, 0.24f));
	Arrow->SetRelativeLocation(FVector(0.0f, -444.0f, 0.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass
	(TEXT("/Game/_Content/BP/UI/TestWarning/WBP_Arrow"));
	if (WidgetClass.Succeeded())
	{
		Arrow->SetWidgetClass(WidgetClass.Class);
	}
	
	Body = CreateDefaultSubobject<UWidgetComponent>(TEXT("Body"));
	Body->SetupAttachment(RootComponent);
	Body->SetWidgetSpace(EWidgetSpace::World);
	Body->SetDrawSize(FVector2D(500.0f, 500.0f));
	Body->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassBody
	(TEXT("/Game/_Content/BP/UI/TestWarning/WBP_SideWarning"));
	if (WidgetClassBody.Succeeded())
	{
		Body->SetWidgetClass(WidgetClassBody.Class);
	}
}


// Called when the game starts or when spawned
void ABaseWarningActor::BeginPlay()
{
	Super::BeginPlay();
	if(!DataHit.bBlockingHit || !RootArrow->IsValidLowLevel())
		Destroy();
	//APlayerCameraManager *temp = Cast<APlayerCameraManager>(GetWorld()->GetFirstPlayerController());
	APlayerCameraManager *temp = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	if(temp->IsValidLowLevel())
	{
		CameraManagerRef = temp;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Camera found!"));
	} else
	{
		CameraManagerRef = nullptr;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Camera not found!"));
	}
}

// Called every frame
void ABaseWarningActor::Tick(float DeltaTime)
{
	// Face camera
	if(CameraManagerRef->IsValidLowLevel())
	{
		FRotator refRota = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CameraManagerRef->GetCameraLocation());
		SetActorRotation(refRota);
	}
	// Set Arrow
	FRotator MyRotator = FRotationMatrix::MakeFromX(DataHit.Normal).Rotator();
	RootArrow->SetWorldRotation(MyRotator);
	Super::Tick(DeltaTime);
	// Check Color
	CheckDistanceColor();
}


void ABaseWarningActor::CheckDistanceColor()
{
	float distance = GetDistanceTo(OwnerObstacle);
	GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple,
		FString::Printf(TEXT("Distance of %f between obstacle and warning "),
			distance));
	if(distance < 10.0f)
		Destroy();
}
