// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FreeFallCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraActor.h"
#include "Characters/FreeFallCharacterInputData.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Obstacle/Obstacle.h"
#include "Other/DiveLevels.h"
#include "Other/Parachute.h"


// Sets default values
AFreeFallCharacter::AFreeFallCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create & setup collision
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	if(CapsuleCollision != nullptr)
	{
		//Setup capsule
		CapsuleCollision->InitCapsuleSize(40,88);
		CapsuleCollision->SetRelativeRotation(FRotator(0,90,0));
		
		//Attach
		CapsuleCollision->SetupAttachment(RootComponent); // RootComponent est généralement la capsule

		//Setup collisions
		CapsuleCollision->SetCollisionProfileName(TEXT("BlockAllDynamic")); // Bloque tous les objets dynamiques
		CapsuleCollision->SetNotifyRigidBodyCollision(true); // Permet de recevoir des notifications de collisions
		CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	//Create grabbing point
	ObjectGrabPoint = CreateDefaultSubobject<USceneComponent>("GrabPoint");
	if(ObjectGrabPoint != nullptr)
	{
		ObjectGrabPoint->SetupAttachment(RootComponent);
	}

	//Create Parachute attach point
	ParachuteAttachPoint = CreateDefaultSubobject<USceneComponent>("Parachute");
	if(ParachuteAttachPoint != nullptr)
	{
		ParachuteAttachPoint->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AFreeFallCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DiveLevelsActor = Cast<ADiveLevels>(UGameplayStatics::GetActorOfClass(GetWorld(), ADiveLevels::StaticClass()));
	
	//Setup state machine
	CreateStateMachine();
	InitStateMachine();

	//Setup movement & physics
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCapsuleComponent()->SetSimulatePhysics(false);

	if(CapsuleCollision)
	{
		CapsuleCollision->OnComponentHit.AddDynamic(this, &AFreeFallCharacter::OnCapsuleCollisionHit);
	}

	//TODO: Pass this in Gamemode to clean code
	CameraActor = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));
}

// Called every frame
void AFreeFallCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickStateMachine(DeltaTime);

	//Update physic based on grab
	switch (GrabbingState)
	{
	case EFreeFallCharacterGrabbingState::None:
	case EFreeFallCharacterGrabbingState::GrabHeavierObject:
		break;
	case EFreeFallCharacterGrabbingState::GrabPlayer:
		UpdateMovementInfluence(DeltaTime);
		break;
	case EFreeFallCharacterGrabbingState::GrabObject:
		UpdateObjectPosition(DeltaTime);
		break;
	}
	
}

void AFreeFallCharacter::DestroyPlayer()
{
	LaunchParachute();
	Destroy();
}

#pragma region StateMachine & IMC
// Called to bind functionality to input
void AFreeFallCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupMappingContextIntoController();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr) return;

	BindInputMoveAxisAndActions(EnhancedInputComponent);
	BindInputDiveAxisAndActions(EnhancedInputComponent);
	BindInputGrabActions(EnhancedInputComponent);
}


void AFreeFallCharacter::CreateStateMachine()
{
	StateMachine = NewObject<UFreeFallCharacterStateMachine>(this);
}

void AFreeFallCharacter::InitStateMachine()
{
	if (StateMachine == nullptr) return;
	StateMachine->Init(this);
}

void AFreeFallCharacter::TickStateMachine(float DeltaTime) const
{
	if (StateMachine == nullptr) return;
	StateMachine->Tick(DeltaTime);
}

TObjectPtr<UFreeFallCharacterStateMachine> AFreeFallCharacter::GetStateMachine() const
{
	return StateMachine;
}

void AFreeFallCharacter::SetupMappingContextIntoController() const
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr) return;

	ULocalPlayer* Player = PlayerController->GetLocalPlayer();
	if (Player == nullptr) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = Player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSystem == nullptr) return;

	InputSystem->AddMappingContext(InputMappingContext,0);
}
#pragma endregion

#pragma region Move
FVector2D AFreeFallCharacter::GetInputMove() const
{
	if(GrabbingState == EFreeFallCharacterGrabbingState::GrabHeavierObject) return FVector2D::ZeroVector;
	
	return InputMove;
}

void AFreeFallCharacter::BindInputMoveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent)
{

	if (InputData == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("InputData est nullptr pour " + GetFName().ToString()));
		return;
	};

	if (InputData->InputActionMove)
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionMove,
			ETriggerEvent::Started,
			this,
			&AFreeFallCharacter::OnInputMove
			);

		EnhancedInputComponent->BindAction(
			InputData->InputActionMove,
			ETriggerEvent::Triggered,
			this,
			&AFreeFallCharacter::OnInputMove
			);

		EnhancedInputComponent->BindAction(
			InputData->InputActionMove,
			ETriggerEvent::Completed,
			this,
			&AFreeFallCharacter::OnInputMove
			);
	}
}

void AFreeFallCharacter::OnInputMove(const FInputActionValue& Value)
{
	InputMove = Value.Get<FVector2D>();
}
#pragma endregion

#pragma region Dive
float AFreeFallCharacter::GetInputDive() const
{
	if(GrabbingState == EFreeFallCharacterGrabbingState::GrabHeavierObject) return 0.0f;
	return InputDive;
}

void AFreeFallCharacter::SetDiveMaterialColor()
{
	if (DiveMaterialInstance == nullptr || DiveLevelsActor == nullptr) return;
	EDiveLayersID DiveLayer = DiveLevelsActor->GetDiveLayersFromCoord(GetActorLocation().Z);
	if (DiveLevelsColors.Contains(DiveLayer))
	{
		DiveMaterialInstance->SetVectorParameterValue("MaterialColor", DiveLevelsColors[DiveLayer]);
	}
}

ADiveLevels* AFreeFallCharacter::GetDiveLevelsActor() const
{
	return DiveLevelsActor;
}

float AFreeFallCharacter::GetDiveLayerForceStrength() const
{
	return DiveLayerForceStrength;
}

ACameraActor* AFreeFallCharacter::GetCameraActor() const
{
	return CameraActor;
}

void AFreeFallCharacter::BindInputDiveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent)
{

	if (InputData == nullptr) return;
	
	if (InputData->InputActionDive)
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionDive,
			ETriggerEvent::Started,
			this,
			&AFreeFallCharacter::OnInputDive
			);

		EnhancedInputComponent->BindAction(
			InputData->InputActionDive,
			ETriggerEvent::Triggered,
			this,
			&AFreeFallCharacter::OnInputDive
			);

		EnhancedInputComponent->BindAction(
			InputData->InputActionDive,
			ETriggerEvent::Completed,
			this,
			&AFreeFallCharacter::OnInputDive
			);
	}
}

void AFreeFallCharacter::OnInputDive(const FInputActionValue& Value)
{
	InputDive = Value.Get<float>();
}
#pragma endregion 

#pragma region Grabbing
void AFreeFallCharacter::BindInputGrabActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData == nullptr) return;
	
	if (InputData->InputActionGrab)
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionGrab,
			ETriggerEvent::Started,
			this,
			&AFreeFallCharacter::OnInputGrab
			);

		EnhancedInputComponent->BindAction(
			InputData->InputActionGrab,
			ETriggerEvent::Completed,
			this,
			&AFreeFallCharacter::OnInputGrab
			);
	}
}

void AFreeFallCharacter::OnInputGrab(const FInputActionValue& Value)
{
	bInputGrabPressed = Value.Get<bool>();
	OnInputGrabEvent.Broadcast();
}

void AFreeFallCharacter::UpdateMovementInfluence(float DeltaTime) const
{
	if(OtherCharacter == nullptr) return;

	//Calculate new offset of child actor based on Character rotation
	if(GrabbingState == EFreeFallCharacterGrabbingState::GrabPlayer)
	{
		FVector RotatedOffset = this->GetActorRotation().RotateVector(GrabInitialOffset);
		FVector NewOtherCharacterPosition = this->GetActorLocation() + RotatedOffset;
		OtherCharacter->SetActorLocation(NewOtherCharacterPosition);		
	}
	
    //Get both players velocity
    FVector CharacterVelocity = GetVelocity();
    FVector OtherCharacterVelocity = OtherCharacter->GetVelocity();
	
	//Mutual influence of movements
	FVector DirectionToOther = (OtherCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector CombinedMovement = (CharacterVelocity + OtherCharacterVelocity) * 0.5f;
	FVector PerpendicularForce = FVector::CrossProduct(CharacterVelocity, DirectionToOther) * GrabRotationInfluenceStrength;
    	
	//Calculate new velocities based on combined forces
	FVector NewCharacterVelocity = CombinedMovement + PerpendicularForce * DeltaTime;
	FVector NewOtherCharacterVelocity = CombinedMovement - PerpendicularForce * DeltaTime;
	//Apply each other's velocity
	GetMovementComponent()->Velocity = NewCharacterVelocity;
	OtherCharacter->GetMovementComponent()->Velocity = NewOtherCharacterVelocity;

	//Set other Character rotation
	FRotator TargetRotation = this->GetActorRotation();
	TargetRotation += GrabDefaultRotationOffset;
	FRotator NewGrabbedRotation = FMath::RInterpTo(OtherCharacter->GetActorRotation(), TargetRotation, DeltaTime, GrabRotationSpeed);
	OtherCharacter->SetActorRotation(NewGrabbedRotation);
}

void AFreeFallCharacter::UpdateObjectPosition(float DeltaTime) const
{
	OtherObject->SetActorLocation(GetObjectGrabPoint()->GetComponentLocation());
}

void AFreeFallCharacter::UpdateHeavyObjectPosition(float DeltaTime)
{

	FVector NewPosition = OtherObject->GetActorLocation() + GrabHeavyObjectRelativeLocationPoint;
	SetActorLocation(NewPosition);
}

TObjectPtr<USceneComponent> AFreeFallCharacter::GetObjectGrabPoint() const
{
	return ObjectGrabPoint;
}

#pragma endregion

#pragma region Bounce Fucntions
void AFreeFallCharacter::BounceCooldown()
{
	bAlreadyCollided = true;

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AFreeFallCharacter::ResetBounce, BounceCooldownDelay, false);
}

float AFreeFallCharacter::GetPlayerMass()
{
	return PlayerMass;
}

void AFreeFallCharacter::ResetBounce()
{
	bAlreadyCollided = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AFreeFallCharacter::OnCapsuleCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(bAlreadyCollided) return; //Return if cooldown isn't over
	
	//Cast to smaller obstacle
	AObstacle* OtherObstacle = Cast<AObstacle>(OtherActor);
	if(OtherObstacle)
	{
		// Récupération du mesh de l'obstacle
		UStaticMeshComponent* ObstacleMesh = OtherObstacle->GetMesh();

		//Get impact direction
		FVector ImpactDirection = (ObstacleMesh->GetComponentLocation() - GetActorLocation()).GetSafeNormal();

		//Player bounce
		FVector PlayerVelocity = (-ImpactDirection * ObstacleMesh->GetPhysicsLinearVelocity().Size() * BounceObstacleRestitutionMultiplier
			* (bShouldConsiderMassObject ? ObstacleMesh->GetMass() / PlayerMass : 1)
			+ (bShouldKeepRemainingVelocity ? GetCharacterMovement()->Velocity * (1 - BouncePlayerRestitutionMultiplier) : FVector::Zero())) 
			* BouncePlayerMultiplier;
		
		PlayerVelocity.Z = 0;
		LaunchCharacter(PlayerVelocity, true, true);

		//Obstacle Bounce
		FVector ObjectVelocity = (ImpactDirection * GetCharacterMovement()->Velocity.Size() * BouncePlayerRestitutionMultiplier
			* (bShouldConsiderMassObject ? PlayerMass / ObstacleMesh->GetMass() : 1)
			+ (bShouldKeepRemainingVelocity ? ObstacleMesh->GetPhysicsLinearVelocity() * (1 - BounceObstacleRestitutionMultiplier) : FVector::Zero())) 
			* BounceObstacleMultiplier;
		
		ObjectVelocity.Z = 0;
		ObstacleMesh->AddForce(ObjectVelocity);
	}

	//Cast to bigger FreefallCharacter
	AFreeFallCharacter* OtherFreeFallCharacter = Cast<AFreeFallCharacter>(OtherActor);
	if(OtherFreeFallCharacter)
	{
		// //Launch character logic
		// OldVelocity = GetCharacterMovement()->Velocity;
		//
		// //Bounce self 
		// FVector NewVelocity = (OtherFreeFallCharacter->GetCharacterMovement()->Velocity * BouncePlayerRestitutionMultiplier
		// 	+ (bShouldKeepRemainingVelocity ? GetCharacterMovement()->Velocity * (1-BouncePlayerRestitutionMultiplier) : FVector::Zero()) )
		// 	* BouncePlayerMultiplier;
		// LaunchCharacter(NewVelocity, true, true);
		//
		// GEngine->AddOnScreenDebugMessage(
		// -1,
		// 3.f,
		// FColor::Red,
		// TEXT("Between Char Velocity = " + NewVelocity.ToString())
		// );
		//
		// //Bounce other character
		// NewVelocity = (OldVelocity * BouncePlayerRestitutionMultiplier
		// 	+ (bShouldKeepRemainingVelocity ? OtherFreeFallCharacter->GetCharacterMovement()->Velocity * (1-BouncePlayerRestitutionMultiplier) : FVector::Zero()) )
		// 	* BouncePlayerMultiplier;
		// OtherFreeFallCharacter->LaunchCharacter(NewVelocity, true, true);
		//
		// if(!OtherFreeFallCharacter->bAlreadyCollided)
		// {
		// 	OtherFreeFallCharacter->BounceCooldown();
		// }

		//Get impact direction
		FVector ImpactDirection = (OtherFreeFallCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		
		OldVelocity = GetCharacterMovement()->Velocity;

		//Bounce self
		FVector NewVelocity = (-ImpactDirection * OtherFreeFallCharacter->GetCharacterMovement()->Velocity.Size() * BouncePlayerRestitutionMultiplier
			+ (bShouldKeepRemainingVelocity ? OldVelocity * (1 - BouncePlayerRestitutionMultiplier) : FVector::Zero()))
			* BouncePlayerMultiplier;
		LaunchCharacter(NewVelocity, true, true);
		
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.f,
			FColor::Red,
			TEXT("Between Char Velocity = " + NewVelocity.ToString())
		);

		//Bounce other character
		NewVelocity = (ImpactDirection * OldVelocity.Size() * BouncePlayerRestitutionMultiplier
			+ (bShouldKeepRemainingVelocity ? OtherFreeFallCharacter->GetCharacterMovement()->Velocity * (1 - BouncePlayerRestitutionMultiplier) : FVector::Zero()))
			* BouncePlayerMultiplier;
		OtherFreeFallCharacter->LaunchCharacter(NewVelocity, true, true);
		
		if (!OtherFreeFallCharacter->bAlreadyCollided)
		{
			OtherFreeFallCharacter->BounceCooldown();
		}
	}

	BounceCooldown();
}
#pragma endregion 

#pragma region Parachute Fucntions

USceneComponent* AFreeFallCharacter::GetParachuteAttachPoint()
{
	return ParachuteAttachPoint;
}

void AFreeFallCharacter::LaunchParachute()
{
	if(!Parachute) return;
	AParachute* LooseParachute = Parachute->DropParachute(this);

	FVector DirectionToCenter = (GetActorLocation() - FVector(0, 0, 0)).GetSafeNormal();
	
	LooseParachute->LaunchParacute(DirectionToCenter);
}

#pragma endregion
