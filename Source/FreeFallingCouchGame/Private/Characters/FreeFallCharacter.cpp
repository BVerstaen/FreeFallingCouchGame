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
#include "PowerUps/PowerUpObject.h"


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
	case EFreeFallCharacterGrabbingState::GrabPlayer:
		break;
	case EFreeFallCharacterGrabbingState::GrabHeavierObject:
		UpdateHeavyObjectPosition(DeltaTime);
		break;
	case EFreeFallCharacterGrabbingState::GrabObject:
		UpdateObjectPosition(DeltaTime);
		break;
	}

	for (TObjectPtr<UPowerUpObject> PowerUpObject : UsedPowerUps)
	{
		PowerUpObject->Tick(DeltaTime);
		if (PowerUpObject->bIsActionFinished)
		{
			PowerUpObject->PrepareForDestruction();
			UsedPowerUps.Remove(PowerUpObject);
		}
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

	//Remove reference if was grabbing
	if(OtherCharacterGrabbing)
	{
		OtherCharacterGrabbing->OtherCharacterGrabbedBy = nullptr;
		OtherCharacterGrabbing = nullptr;
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
	BindInputUsePowerUpActions(EnhancedInputComponent);
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
float AFreeFallCharacter::GetInputDive()
{
	//Can't dive if is grabbing a heavier object
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

ACameraActor* AFreeFallCharacter::GetCameraActor() const
{
	if(!CameraActor)
		return nullptr;
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
	//Invert dive input
	if(InvertDiveInput)
		InputDive *= -1;
}

#pragma endregion

#pragma region DiveLayerSensible Interface

void AFreeFallCharacter::ApplyDiveForce(FVector DiveForceDirection, float DiveStrength)
{
	GetCharacterMovement()->bOrientRotationToMovement = (GrabbingState != EFreeFallCharacterGrabbingState::GrabHeavierObject);
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
		OtherCharacter->SetActorLocation(NewOtherCharacterPosition, true);
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
	if(!(OtherCharacterGrabbedBy == OtherCharacter) && !bIsCircularGrab)
	{
		FRotator TargetRotation = this->GetActorRotation();
		TargetRotation += GrabDefaultRotationOffset;
		FRotator NewGrabbedRotation = FMath::RInterpTo(OtherCharacter->GetActorRotation(), TargetRotation, DeltaTime, GrabRotationSpeed);

		GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Cyan, GetName() + " - " + TargetRotation.ToString());

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

	//Update dissociation problem if there's any
	if(OtherCharacterGrabbing)
	{
		UpdateDissociationProblems(DeltaTime);
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

void AFreeFallCharacter::UpdateDissociationProblems(float DeltaTime)
{
	//Check if still has grabbing player in front of me
	FTransform CharacterTransform = GetTransform();
	FVector SphereLocation = CharacterTransform.GetLocation() + CharacterTransform.GetRotation().GetForwardVector() * 150;
	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	TArray<AActor*> ignoreActors;
	FHitResult HitResult;
	
	bool CanGrab = UKismetSystemLibrary::SphereTraceSingleForObjects(
														GetWorld(),
														SphereLocation,
														SphereLocation,
														20,
														traceObjectTypes,
														false,
														ignoreActors,
														EDrawDebugTrace::ForOneFrame,
														HitResult,
														true);

	if(!CanGrab || OtherCharacterGrabbing != HitResult.GetActor())
	{
		//If notice any dissociation problem -> then launch character to free character space in front of him.
		FVector DissociationFeedbackDirection = OtherCharacterGrabbing->GetActorLocation() - GetActorLocation();
		DissociationFeedbackDirection *= -.8;
		LaunchCharacter(DissociationFeedbackDirection, false, false);
	}
}

bool AFreeFallCharacter::IsLookingToCloseToGrabber(float AngleLimit)
{
	if(!OtherCharacterGrabbedBy || !OtherCharacterGrabbing) return false;

	float SelfYRotation = GetActorRotation().Yaw;
	float OtherYRotation = OtherCharacterGrabbedBy->GetActorRotation().Yaw;
	float LookDiffAngle = FMath::Abs(OtherYRotation - SelfYRotation);

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, GetName() + " - " + (LookDiffAngle > 180.f - AngleLimit && LookDiffAngle < 180 + AngleLimit ? "Yes" : "No"));
	
	return LookDiffAngle > 180.f - AngleLimit && LookDiffAngle < 180 + AngleLimit;
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

float AFreeFallCharacter::GetMass()
{
	return PlayerMass;
}

FVector AFreeFallCharacter::GetVelocity()
{
	return GetCharacterMovement()->Velocity;
}

EBounceParameters AFreeFallCharacter::GetBounceParameterType()
{
	return Player;
}

void AFreeFallCharacter::AddBounceForce(FVector Velocity)
{
	LaunchCharacter(Velocity, true, true);
}

AFreeFallCharacter* AFreeFallCharacter::CollidedWithPlayer()
{
	return this;
}

void AFreeFallCharacter::BounceRoutine(AActor* OtherActor, TScriptInterface<IBounceableInterface> OtherBounceableInterface, float SelfRestitutionMultiplier, float OtherRestitutionMultiplier, float GlobalMultiplier,
                                       bool bShouldConsiderMass, bool bShouldKeepRemainVelocity)
{
	//Get old velocity
	FVector OldVelocity = GetVelocity();
	
	//Get impact direction
	FVector ImpactDirection = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal(); 

	//Bounce self
	//Player bounce
	FVector NewVelocity = (-ImpactDirection * OtherBounceableInterface->GetVelocity().Size() * OtherRestitutionMultiplier
		* (bShouldConsiderMass ? OtherBounceableInterface->GetMass() / GetMass() : 1)
		+ (bShouldKeepRemainVelocity ? OldVelocity * (1 - SelfRestitutionMultiplier) : FVector::Zero())) 
		* GlobalMultiplier;
	//Neutralize Z bounce velocity
	NewVelocity.Z = 0;
	AddBounceForce(NewVelocity);

	//Bounce other character
	NewVelocity = (ImpactDirection * OldVelocity.Size() * SelfRestitutionMultiplier
			* (bShouldConsiderMass ? GetMass() / OtherBounceableInterface->GetMass() : 1)
			+ (bShouldKeepRemainVelocity ? OtherBounceableInterface->GetVelocity() * (1 - OtherRestitutionMultiplier) : FVector::Zero())) 
			* GlobalMultiplier;
	//Neutralize Z bounce velocity
	NewVelocity.Z = 0;
	OtherBounceableInterface->AddBounceForce(NewVelocity);
	
	//Check if collided with players
	if(AFreeFallCharacter* OtherFreeFallCharacter = OtherBounceableInterface->CollidedWithPlayer())
	{
		//Activate bounce cooldown & elimination timers
		if (!OtherFreeFallCharacter->bAlreadyCollided)
			OtherFreeFallCharacter->BounceCooldown();
		
		SetWasRecentlyBouncedTimer(OtherFreeFallCharacter);
		OtherFreeFallCharacter->SetWasRecentlyBouncedTimer(this);

		BounceCooldown();
	}
}


void AFreeFallCharacter::OnCapsuleCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(bAlreadyCollided) return; //Return if cooldown isn't over

	//Check if have bouncable interface
	if(!OtherActor->Implements<UBounceableInterface>()) return;
	TScriptInterface<IBounceableInterface> OtherBounceableInterface = TScriptInterface<IBounceableInterface>(OtherActor);
	
	switch (OtherBounceableInterface->GetBounceParameterType())
	{
	case Obstacle:
		BounceRoutine(OtherActor, OtherBounceableInterface, BouncePlayerRestitutionMultiplier,
			BounceObstacleRestitutionMultiplier, BounceObstacleMultiplier, bShouldConsiderMassObject, bShouldKeepRemainingVelocity);
		break;
	case Player:
		BounceRoutine(OtherActor, OtherBounceableInterface, BouncePlayerRestitutionMultiplier,
	BouncePlayerRestitutionMultiplier, BouncePlayerMultiplier, true, bShouldKeepRemainingVelocity);
		break;
	}
	
}


#pragma endregion 

#pragma region Parachute Fucntions

USceneComponent* AFreeFallCharacter::GetParachuteAttachPoint()
{
	return ParachuteAttachPoint;
}

#pragma endregion

#pragma region PowerUp Fuctions

void AFreeFallCharacter::SetPowerUp(UPowerUpObject* PowerUpObject)
{
	if (CurrentPowerUp != nullptr) CurrentPowerUp->PrepareForDestruction();
	CurrentPowerUp = PowerUpObject;
	OnTakePowerUp.Broadcast(this);
}

void AFreeFallCharacter::BindInputUsePowerUpActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData == nullptr) return;
	
	if (InputData->InputActionUsePowerUp)
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionUsePowerUp,
			ETriggerEvent::Started,
			this,
			&AFreeFallCharacter::OnInputUsePowerUp
			);

		EnhancedInputComponent->BindAction(
			InputData->InputActionUsePowerUp,
			ETriggerEvent::Completed,
			this,
			&AFreeFallCharacter::OnInputUsePowerUp
			);
	}
}

void AFreeFallCharacter::OnInputUsePowerUp(const FInputActionValue& Value)
{
	OnInputUsePowerUpEvent.Broadcast();
}

#pragma endregion
