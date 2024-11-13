// Fill out your copyright notice in the Description page of Project Settings.
#include "UI/BaseWarningActor.h"

// Sets default values
ABaseWarningActor::ABaseWarningActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootBody = CreateDefaultSubobject<USceneComponent>(TEXT("RootMain"));
	RootComponent = RootBody;
	SetupWidgets();
}

// Called when the game starts or when spawned
void ABaseWarningActor::BeginPlay()
{
	Super::BeginPlay();
	if(!DataHit.bBlockingHit /*|| !RootArrow->IsValidLowLevel()*/)
		Destroy();
	APlayerCameraManager *temp = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	if((DirectionObject.Z > 0.5 || DirectionObject.Z < -0.5)
		&& (DirectionObject.X == 0.0f && DirectionObject.Y == 0) )
	{
		WarningType = EWarningType::Vertical;
		OverwriteWidgets();
	}
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

void ABaseWarningActor::SetupWidgets()
{
	Arrow = CreateDefaultSubobject<UWidgetComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(RootBody);
	Arrow->SetWidgetSpace(EWidgetSpace::World);
	Arrow->SetDrawSize(FVector2D(500.0f, 500.0f));
	Arrow->SetRelativeScale3D(FVector(0.24f, 0.24f, 0.24f));
	Arrow->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass
	(TEXT("/Game/_Content/BP/UI/TestWarning/WBP_Arrow"));
	if (WidgetClass.Succeeded())
	{
		Arrow->SetWidgetClass(WidgetClass.Class);
	}

	
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassBody
	(TEXT("/Game/_Content/BP/UI/TestWarning/WBP_SideWarning"));
	if(WidgetClassBody.Succeeded())
	{
		CachedWidgetClass = WidgetClassBody.Class;
	}
}

void ABaseWarningActor::OverwriteWidgets() const
{
	if(WarningType == EWarningType::Vertical)
	{
		Arrow->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
		Arrow->SetWidgetClass(CachedWidgetClass);
	}
}

void  ABaseWarningActor::FaceCamera()
{
	FRotator refRota2 = CameraManagerRef->GetCameraRotation();
	FQuat refQuat = FQuat(refRota2);
	refQuat = refQuat.Inverse();
	SetActorRotation(refQuat.Rotator());
}

// Called every frame
void ABaseWarningActor::Tick(float DeltaTime)
{
	FaceCamera();
	// Set rota arrow
	switch (WarningType)
	{
	case EWarningType::Base:
		{
			FVector direction = OwnerObstacle->GetActorLocation() - GetActorLocation();
			float angle = FMath::Atan2(direction.X, -direction.Y);
			angle = FMath::RadiansToDegrees(angle);
			FRotator MyRotator(0, 0, -angle);
			Arrow->SetRelativeRotation(MyRotator);
			break;
		}
	case EWarningType::Vertical:
		FRotator MyRotator(0, 0, 180.0f);
		Arrow->SetRelativeRotation(MyRotator);
		break;
	}
	// Check Color
	CheckDistanceColor();
	Super::Tick(DeltaTime);
}

void ABaseWarningActor::CheckDistanceColor()
{
	float distance = GetDistanceTo(OwnerObstacle);
	//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(
	//	TEXT("Distance of %f between obstacle and warning "),distance));

	if(distance >= 1000) {
		Arrow->SetTintColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.0f));
	} else if (distance >= 500 && distance <= 999) {
		Arrow->SetTintColorAndOpacity(FLinearColor(1.0f, 0.5f, 0.1f));
	} else if(distance >= 200 && distance <= 499) {
		Arrow->SetTintColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f));
	}
	if(distance < 200.0f)
		Destroy();
}
