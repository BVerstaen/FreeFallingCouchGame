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

	CameraActor = Cast<ACameraActor>(UGameplayStatics::GetActorOfClass(GetWorld(), ACameraActor::StaticClass()));

	//Setup movement & physics
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	GetCapsuleComponent()->SetSimulatePhysics(false);

	if(CapsuleCollision)
	{
		CapsuleCollision->OnComponentHit.AddDynamic(this, &AFreeFallCharacter::OnCapsuleCollisionHit);
	}
	
}

// Called every frame
void AFreeFallCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickStateMachine(DeltaTime);
	UpdateEveryMovementInfluence(DeltaTime);
	
	//TODO: Delete that when Shader is created
	SetDiveMaterialColor();
	
	//Update physic based on grab
	switch (GrabbingState)
	{
	case EFreeFallCharacterGrabbingState::None:
	case EFreeFallCharacterGrabbingState::GrabHeavierObject:
	case EFreeFallCharacterGrabbingState::GrabPlayer:
		break;
	case EFreeFallCharacterGrabbingState::GrabObject:
		UpdateObjectPosition(DeltaTime);
		break;
	}
	
}

void AFreeFallCharacter::DestroyPlayer()
{
	//If was recently bounced -> then send elimination delegate
	if(bWasRecentlyBounced)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Player" + FString::FromInt(getIDPlayerLinked()) + " - got killed by - " + FString::FromInt(RecentlyBouncedOtherPlayerID));
		if(OnWasEliminated.IsBound())
			OnWasEliminated.Broadcast(this, RecentlyBouncedOtherPlayer);
	}
	else
	{
		if(Parachute)
		{
			Parachute->DropParachute(this);
		}
	}
	
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

#pragma region DiveLayerSensible Interface

void AFreeFallCharacter::ApplyDiveForce(FVector DiveForceDirection, float DiveStrength)
{
	AddMovementInput(DiveForceDirection,DiveStrength / GetCharacterMovement()->MaxFlySpeed);
}

EDiveLayersID AFreeFallCharacter::GetBoundedLayer()
{
	return BoundedLayer;
}

AActor* AFreeFallCharacter::GetSelfActor()
{
	return this;
}

FVector AFreeFallCharacter::GetDivingVelocity()
{
	return DivingVelocity;
}

bool AFreeFallCharacter::IsBoundedByLayer()
{
	return bIsBoundedByLayer;
}

bool AFreeFallCharacter::IsDiveForced()
{
	return bIsDiveForced;
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

bool AFreeFallCharacter::IsInCircularGrab()
{
	AFreeFallCharacter* CurrentCharacter = OtherCharacterGrabbing;
	if(CurrentCharacter)
	{
		//If two char are grabbing -> don't check
		if (CurrentCharacter->OtherCharacterGrabbing)
		{
			CurrentCharacter = CurrentCharacter->OtherCharacterGrabbing;
			//Check if one char of the chain grab is the one I'm grabbing
			while (CurrentCharacter != nullptr)
			{
				if (CurrentCharacter->OtherCharacterGrabbing == this)
				{
					if(GrabCircularRotationOffset == FRotator::ZeroRotator)
					{
						GrabCircularRotationOffset = GetActorRotation();
					}
					return true;
				}
				CurrentCharacter = CurrentCharacter->OtherCharacterGrabbing;
			}
		}
	}
	
	GrabCircularRotationOffset = FRotator::ZeroRotator;
	return false;
}

void AFreeFallCharacter::UpdateMovementInfluence(float DeltaTime, AFreeFallCharacter* OtherCharacter, bool bIsCircularGrab)
{
	//Calculate new offset of child actor based on Character rotation
	if(OtherCharacterGrabbing == OtherCharacter && !bIsCircularGrab)
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
	if(!(OtherCharacterGrabbedBy == OtherCharacter && OtherCharacterGrabbing) && !bIsCircularGrab)
	{
		FRotator TargetRotation = this->GetActorRotation();
		TargetRotation += GrabDefaultRotationOffset;
		FRotator NewGrabbedRotation = FMath::RInterpTo(OtherCharacter->GetActorRotation(), TargetRotation, DeltaTime, GrabRotationSpeed);
		OtherCharacter->SetActorRotation(NewGrabbedRotation);
	}
	//Set self rotation if is in a circular grab
	else if(bIsCircularGrab)
	{
		//Calculate Stabilized rotation -> based on GrabCircularRotationOffset
		float MaxRotationDeviation = 15.0f;  
		FRotator StabilizedRotation = GrabCircularRotationOffset;
		StabilizedRotation.Yaw = FMath::Clamp(GetActorRotation().Yaw, 
											  GrabCircularRotationOffset.Yaw - MaxRotationDeviation,
											  GrabCircularRotationOffset.Yaw + MaxRotationDeviation);
        
		//Interpolation toward the stabilized rotation within clamped range
		FRotator NewStabilizedRotation = FMath::RInterpTo(GetActorRotation(), StabilizedRotation, DeltaTime, GrabRotationSpeed);
		SetActorRotation(NewStabilizedRotation);
	}
}

void AFreeFallCharacter::UpdateEveryMovementInfluence(float DeltaTime)
{
	bool bIsInCircularGrab = IsInCircularGrab();
	
	if(OtherCharacterGrabbedBy)
		UpdateMovementInfluence(DeltaTime, OtherCharacterGrabbedBy, bIsInCircularGrab);
	if(OtherCharacterGrabbing)
		UpdateMovementInfluence(DeltaTime, OtherCharacterGrabbing, bIsInCircularGrab);
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

void AFreeFallCharacter::SetWasRecentlyBouncedTimer(AFreeFallCharacter* Character)
{
	bWasRecentlyBounced = true;
	RecentlyBouncedOtherPlayer = Character;
	GetWorldTimerManager().ClearTimer(RecentlyBouncedTimer);
	GetWorldTimerManager().SetTimer(RecentlyBouncedTimer, this, &AFreeFallCharacter::ResetWasRecentlyBounced, DelayConsideredAsRecentlyBounced);
}

void AFreeFallCharacter::ResetBounce()
{
	bAlreadyCollided = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AFreeFallCharacter::ResetWasRecentlyBounced()
{
	GetWorldTimerManager().ClearTimer(RecentlyBouncedTimer);
	bWasRecentlyBounced = false;
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
		//Get impact direction
		FVector ImpactDirection = (OtherFreeFallCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		
		OldVelocity = GetCharacterMovement()->Velocity;

		//Bounce self
		FVector NewVelocity = (-ImpactDirection * OtherFreeFallCharacter->GetCharacterMovement()->Velocity.Size() * BouncePlayerRestitutionMultiplier
			+ (bShouldKeepRemainingVelocity ? OldVelocity * (1 - BouncePlayerRestitutionMultiplier) : FVector::Zero()))
			* BouncePlayerMultiplier;
		//Neutralize Z bounce velocity
		NewVelocity.Z = 0;
		LaunchCharacter(NewVelocity, true, true);

		//Bounce other character
		NewVelocity = (ImpactDirection * OldVelocity.Size() * BouncePlayerRestitutionMultiplier
			+ (bShouldKeepRemainingVelocity ? OtherFreeFallCharacter->GetCharacterMovement()->Velocity * (1 - BouncePlayerRestitutionMultiplier) : FVector::Zero()))
			* BouncePlayerMultiplier;
		//Neutralize Z bounce velocity
		NewVelocity.Z = 0;
		OtherFreeFallCharacter->LaunchCharacter(NewVelocity, true, true);

		//Activate bounce cooldown & elimination timers
		if (!OtherFreeFallCharacter->bAlreadyCollided)
			OtherFreeFallCharacter->BounceCooldown();
		
		SetWasRecentlyBouncedTimer(OtherFreeFallCharacter);
		OtherFreeFallCharacter->SetWasRecentlyBouncedTimer(this);
	}

	BounceCooldown();
}
#pragma endregion 

#pragma region Parachute Fucntions

USceneComponent* AFreeFallCharacter::GetParachuteAttachPoint()
{
	return ParachuteAttachPoint;
}

#pragma endregion
