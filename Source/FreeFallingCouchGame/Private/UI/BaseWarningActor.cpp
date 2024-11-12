// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/BaseWarningActor.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABaseWarningActor::ABaseWarningActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//RootArrow = CreateDefaultSubobject<USceneComponent>(TEXT("RootArrow"));
	RootBody = CreateDefaultSubobject<USceneComponent>(TEXT("RootMain"));
	RootComponent = RootBody;
	SetupWidgets();
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
	/*
	Body = CreateDefaultSubobject<UWidgetComponent>(TEXT("Body"));
	Body->SetupAttachment(this->RootBody);
	Body->SetWidgetSpace(EWidgetSpace::World);
	Body->SetDrawSize(FVector2D(500.0f, 500.0f));
	Body->SetRelativeScale3D(FVector(0.4f, 0.4f, 0.4f));
	Body->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassBody
	(TEXT("/Game/_Content/BP/UI/TestWarning/WBP_SideWarning"));
	if (WidgetClassBody.Succeeded())
	{
		Body->SetWidgetClass(WidgetClassBody.Class);
	}
	*/
}

void ABaseWarningActor::SetupWidget(UWidgetComponent* RefObject, FName inName, USceneComponent* root,
	FVector2D InSize2D, const FVector& InScale, const FVector& InPositionRelative, const TCHAR* InPath)
{
	RefObject = CreateDefaultSubobject<UWidgetComponent>(inName);
	RefObject->SetupAttachment(root);
	RefObject->SetWidgetSpace(EWidgetSpace::World);
	RefObject->SetDrawSize(InSize2D);
	RefObject->SetRelativeScale3D(InScale);
	RefObject->SetRelativeLocation(InPositionRelative);
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(InPath);
	if (WidgetClass.Succeeded())
	{
		RefObject->SetWidgetClass(WidgetClass.Class);
	}
}

// Called when the game starts or when spawned
void ABaseWarningActor::BeginPlay()
{
	Super::BeginPlay();
	if(!DataHit.bBlockingHit /*|| !RootArrow->IsValidLowLevel()*/)
		Destroy();
	//SetRotationBeginning();

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

void  ABaseWarningActor::FaceCamera()
{
	FRotator refRota = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CameraManagerRef->GetCameraLocation());
	//refRota.Roll = GetActorRotation().Roll;
	//refRota.Pitch = GetActorRotation().Pitch;
	SetActorRotation(refRota);
}

// Called every frame
void ABaseWarningActor::Tick(float DeltaTime)
{
	FaceCamera();
	// Set rota arrow
	FVector direction = OwnerObstacle->GetActorLocation() - GetActorLocation();
	float angle = FMath::Atan2(direction.X, direction.Y);
	angle = FMath::RadiansToDegrees(angle);
	FRotator MyRotator(0, 0, angle);
	Arrow->SetRelativeRotation(MyRotator);
	// Check Color
	CheckDistanceColor();
	Super::Tick(DeltaTime);
}

void ABaseWarningActor::CheckDistanceColor()
{
	float distance = GetDistanceTo(OwnerObstacle);
	//GEngine->AddOnScreenDebugMessage(-1, 7.f, FColor::Purple, FString::Printf(TEXT("Distance of %f between obstacle and warning "),distance));
	if(distance < 100.0f)
		Destroy();
}
