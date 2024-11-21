// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FreeFallCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Animation/AnimInstanceProxy.h"
#include "Audio/SoundSubsystem.h"
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
#include "PowerUps/PowerUpsID.h"
#include "Settings/CharactersSettings.h"


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
	PlayerMeshDefaultRotation = GetMesh()->GetRelativeRotation();

	CharactersSettings = GetDefault<UCharactersSettings>();

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

	ApplyMovementFromAcceleration(DeltaTime);

	if (GetCharacterMovement()->MovementMode != MOVE_Flying) GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	
	//Update physic based on grab
	switch (GrabbingState)
	{
	case EFreeFallCharacterGrabbingState::None:
		break;
	case EFreeFallCharacterGrabbingState::GrabPlayer:
		PlayAnimation(MidGrabAnimation, true, false);
		break;
	case EFreeFallCharacterGrabbingState::GrabHeavierObject:
		PlayAnimation(MidGrabAnimation, true, false);
		UpdateHeavyObjectPosition(DeltaTime);
		break;
	case EFreeFallCharacterGrabbingState::GrabObject:
		PlayAnimation(MidGrabAnimation, true, false);
		UpdateObjectPosition(DeltaTime);
		break;
	}

	TArray<TObjectPtr<UPowerUpObject>> PowerUpsToRemove;
	for (TObjectPtr<UPowerUpObject> PowerUpObject : UsedPowerUps)
	{
		PowerUpObject->Tick(DeltaTime);
		if (PowerUpObject->bIsActionFinished)
		{
			PowerUpObject->PrepareForDestruction();
			PowerUpsToRemove.Add(PowerUpObject);
		}
	}
	for (TObjectPtr<UPowerUpObject> PowerUpObject : PowerUpsToRemove)
	{
		UsedPowerUps.Remove(PowerUpObject);
	}
	PowerUpsToRemove.Empty();
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
		OtherCharacterGrabbing->GrabbingState = EFreeFallCharacterGrabbingState::None;
		OtherCharacterGrabbing = nullptr;
	}
	//Remove reference if was grabbed
	if(OtherCharacterGrabbedBy)
	{
		OtherCharacterGrabbedBy->OtherCharacterGrabbing = nullptr;
		OtherCharacterGrabbedBy->GrabbingState = EFreeFallCharacterGrabbingState::None;
		OtherCharacterGrabbedBy = nullptr;
	}

	//Play death sound
	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("VOC_PLR_Death_ST", this, false);
	
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
	BindInputDeGrabActions(EnhancedInputComponent);
	BindInputUsePowerUpActions(EnhancedInputComponent);
	BindInputFastDiveAxisAndActions(EnhancedInputComponent);
}

void AFreeFallCharacter::InterpMeshPlayer(FRotator Destination, float DeltaTime, float DampingSpeed)
{
	GetMesh()->SetRelativeRotation(FMath::RInterpTo(GetMesh()->GetRelativeRotation(), Destination, DeltaTime, FMath::Abs(DampingSpeed)));
}

FRotator AFreeFallCharacter::GetPlayerDefaultRotation()
{
	return PlayerMeshDefaultRotation;
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

void AFreeFallCharacter::SetLockControls(bool lockControls)
{
	bAreControlsBlocked = lockControls;
}

bool AFreeFallCharacter::GetLockControls() const
{
	return  bAreControlsBlocked;
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

void AFreeFallCharacter::ApplyMovementFromAcceleration(float DeltaTime)
{
	Decelerate(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-1,DeltaTime,FColor::Orange, TEXT("AccelerationAlpha : " + AccelerationAlpha.ToString()));
	GEngine->AddOnScreenDebugMessage(-1,DeltaTime,FColor::Orange, TEXT("MaxFlySpeed : " + FString::SanitizeFloat(GetCharacterMovement()->MaxFlySpeed)));
	const float ScaleValue = MovementSpeed / GetCharacterMovement()->MaxFlySpeed;
	AddMovementInput(FVector(
		FMath::Abs(AccelerationAlpha.X) < CharactersSettings->AccelerationThreshold ? 0 : AccelerationAlpha.X,
		FMath::Abs(AccelerationAlpha.Y) < CharactersSettings->AccelerationThreshold ? 0 : AccelerationAlpha.Y,
		0), ScaleValue);

	
	FVector MovementDirection = GetVelocity().GetSafeNormal();
	FVector CharacterDirection = GetActorForwardVector();
	
	//Set Orient Rotation To Movement
	if(bShouldOrientToMovement && GrabbingState != EFreeFallCharacterGrabbingState::GrabHeavierObject)
	{
		//Get angle btw Character & movement direction
		float DotProduct = FVector::DotProduct(MovementDirection, CharacterDirection);
		
		//If Reached orientation Threshold in his grabbing state -> stop orientation and let yourself influenced
		if((DotProduct > OrientationThreshold && OtherCharacterGrabbing)
			|| (DotProduct > GrabbedOrientationThreshold && OtherCharacterGrabbedBy)
			|| IsLookingToCloseToGrabber(GrabToCloseToGrabbedAngle))
		{
			bShouldOrientToMovement = false;
			GrabOldInputDirection = InputMove;
		}
	}
	else if(GrabOldInputDirection != InputMove)
	{
		//If you change direction -> Restore Orient Rotation Movement
		bShouldOrientToMovement = true;
	}

	/*
	//Set mesh movement
	FVector2D CharacterDirection2D = FVector2D(CharacterDirection.GetSafeNormal().X, CharacterDirection.GetSafeNormal().Y);
	float AngleDiff = FMath::Clamp(FVector2d::DotProduct(InputMove.GetSafeNormal(), CharacterDirection2D.GetSafeNormal()) , -1.0f , 1.0f);
	InterpMeshPlayer(FRotator((AngleDiff >= 0 ? 1 : -1) * FMath::Lerp(GetPlayerDefaultRotation().Pitch,MeshMovementRotationAngle, 1-FMath::Abs(AngleDiff)),
		GetMesh()->GetRelativeRotation().Yaw, GetPlayerDefaultRotation().Roll), DeltaTime, MeshMovementDampingSpeed);
	*/
}

void AFreeFallCharacter::Decelerate(float DeltaTime)
{
	if (FMath::Abs(AccelerationAlpha.X) > CharactersSettings->AccelerationThreshold)
	{
		if ((InputMove.X > 0 || FMath::Abs(InputMove.X) < CharactersSettings->InputMoveThreshold) && AccelerationAlpha.X < 0)
		{
			AccelerationAlpha.X = FMath::Min(AccelerationAlpha.X + DecelerationSpeed * DeltaTime, 0);
			GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Orange, TEXT("Decelerating"));
		}
		else if ((InputMove.X < 0 || FMath::Abs(InputMove.X) < CharactersSettings->InputMoveThreshold) && AccelerationAlpha.X > 0)
		{
			AccelerationAlpha.X = FMath::Max(AccelerationAlpha.X - DecelerationSpeed * DeltaTime, 0);
			GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Orange, TEXT("Decelerating"));
		}
	}
	if (FMath::Abs(AccelerationAlpha.Y) > CharactersSettings->AccelerationThreshold)
	{
		if ((InputMove.Y > 0 || FMath::Abs(InputMove.Y) < CharactersSettings->InputMoveThreshold) && AccelerationAlpha.Y < 0)
		{
			AccelerationAlpha.Y = FMath::Min(AccelerationAlpha.Y + DecelerationSpeed * DeltaTime, 0);
			GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Orange, TEXT("Decelerating"));
		}
		else if ((InputMove.Y < 0 || FMath::Abs(InputMove.Y) < CharactersSettings->InputMoveThreshold) && AccelerationAlpha.Y > 0)
		{
			AccelerationAlpha.Y = FMath::Max(AccelerationAlpha.Y - DecelerationSpeed * DeltaTime, 0);
			GEngine->AddOnScreenDebugMessage(-1,DeltaTime, FColor::Orange, TEXT("Decelerating"));
		}
	}
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
	/*
	if (DiveMaterialInstance == nullptr || DiveLevelsActor == nullptr) return;
	EDiveLayersID DiveLayer = DiveLevelsActor->GetDiveLayersFromCoord(GetActorLocation().Z);
	if (DiveLevelsColors.Contains(DiveLayer))
	{
		DiveMaterialInstance->SetVectorParameterValue("MaterialColor", DiveLevelsColors[DiveLayer]);
	}
	*/
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

#pragma region FastDive

float AFreeFallCharacter::GetInputFastDive()
{
	return InputFastDive;
}

void AFreeFallCharacter::BindInputFastDiveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData == nullptr) return;

	if (InputData == nullptr) return;
	
	if (InputData->InputActionFastDive)
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionFastDive,
			ETriggerEvent::Started,
			this,
			&AFreeFallCharacter::OnInputFastDive
			);

		EnhancedInputComponent->BindAction(
			InputData->InputActionFastDive,
			ETriggerEvent::Completed,
			this,
			&AFreeFallCharacter::OnInputFastDive
			);
	}
	
}

void AFreeFallCharacter::OnInputFastDive(const FInputActionValue& Value)
{
	InputFastDive = Value.Get<float>();
	//Invert FastDive input
	if(InvertDiveInput)
		InputFastDive *= -1;
	if (FMath::Abs(InputFastDive) > 0.1) OnInputFastDiveEvent.Broadcast();
	GEngine->AddOnScreenDebugMessage(-1,3.f,FColor::Purple, "Pressing Fast Dive : " + FString::SanitizeFloat(InputFastDive));
}

#pragma endregion

#pragma region DiveLayerSensible Interface

void AFreeFallCharacter::ApplyDiveForce(FVector DiveForceDirection, float DiveStrength)
{
	bShouldOrientToMovement = (GrabbingState != EFreeFallCharacterGrabbingState::GrabHeavierObject);
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
	if(!OtherCharacter) return;
	
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
							//This comparaison -> prevent every character to circle around
	if(!bIsCircularGrab && !(OtherCharacterGrabbing && OtherCharacterGrabbedBy))
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
	{
		UpdateMovementInfluence(DeltaTime, OtherCharacterGrabbing, bIsInCircularGrab);
		GEngine->AddOnScreenDebugMessage(-1,15, FColor::Red, OtherCharacterGrabbing->GetName());
	}
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
														30,
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

#pragma region DeGrabbing

void AFreeFallCharacter::BindInputDeGrabActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData == nullptr) return;
	
	if (InputData->InputActionGrab)
	{
		EnhancedInputComponent->BindAction(
			InputData->InputActionDeGrab,
			ETriggerEvent::Started,
			this,
			&AFreeFallCharacter::OnInputDeGrab
			);
	}
}

void AFreeFallCharacter::OnInputDeGrab(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1,15.0f, FColor::Emerald, "Degrab Input");
	if(!OtherCharacterGrabbedBy) return;

	CurrentNumberOfDeGrabInput--;

	if(CurrentNumberOfDeGrabInput <= 0)
	{
		OtherCharacterGrabbedBy->OtherCharacterGrabbing = nullptr;
		OtherCharacterGrabbedBy->GrabbingState = EFreeFallCharacterGrabbingState::None;
		OtherCharacterGrabbedBy = nullptr;

		StopEffectDeGrab();
	}
}

void AFreeFallCharacter::ActivateDeGrab()
{
	CurrentNumberOfDeGrabInput = MaxNumberOfDeGrabInput;
	ActivateEffectDeGrab();
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
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
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

	//Play Bounce Sound
	USoundSubsystem* SoundSubsystem = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	SoundSubsystem->PlaySound("SFX_PLR_Collision_ST", this, false);

	//Play bounce effect
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BounceEffect.LoadSynchronous(), GetActorLocation());
	
	//Play bounce anim
	PlayAnimation(DamageAnimation, false, true);
		
	//Check if collided with players
	if(AFreeFallCharacter* OtherFreeFallCharacter = OtherBounceableInterface->CollidedWithPlayer())
	{
		//Activate bounce cooldown & elimination timers
		if (!OtherFreeFallCharacter->bAlreadyCollided)
		{
			OtherFreeFallCharacter->BounceCooldown();
		}

		//Play player bounce
		SoundSubsystem->PlaySound("VOC_PLR_Hit", this, true);
		SoundSubsystem->PlaySound("VOC_PLR_Shock_ST", OtherFreeFallCharacter, true);

		//Play random "onomatopé"
		TArray<FName> ExpressionHit = {
			"VOC_PLR_Angry_ST",
			"VOC_PLR_Joy_ST",
			"VOC_PLR_Sad_ST",
			"VOC_PLR_Fight_ST",
			"VOC_PLR_Insult_ST",
			"VOC_PLR_Warning_ST"
		};
		FName ExpressionName = ExpressionHit[FMath::RandRange(0, ExpressionHit.Num() - 1)];
		SoundSubsystem->PlaySound(ExpressionName, this, false);
		
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
	UpdatePowerUpUI(CurrentPowerUp? CurrentPowerUp->GetPowerUpID() : EPowerUpsID::None);
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
	bInputUsePowerUpPressed = Value.Get<bool>();
	OnInputUsePowerUpEvent.Broadcast();
}

#pragma endregion

#pragma region Animation

void AFreeFallCharacter::PlayAnimation(UAnimSequence* Animation, bool Looping, bool BlockUntilEndOfAnim, float AnimationDuration)
{
	if(!Animation) return;
	if(PlayingAnimation == Animation) return;
	
	//Leave to queued if animation's blocked
	if(bBlockNewAnimation)
	{
		QueuedAnimation = Animation;
		QueuedAnimationLooping = Looping;
		return;
	}

	//Play animation
	GetMesh()->PlayAnimation(Animation, Looping);
	PlayingAnimation = Animation;

	//Block new animation & wait until end of anim
	if(BlockUntilEndOfAnim)
	{
		//if negative duration -> use Animation's duration
		if(AnimationDuration <= -1.0f)
			AnimationDuration = Animation->GetPlayLength();

		bBlockNewAnimation = true;
		QueuedAnimation = nullptr;
		GetWorldTimerManager().SetTimer(BlockUntilEndOfAnimTimerHandle, this, &AFreeFallCharacter::RestoreAnimation, AnimationDuration);
	}

}

void AFreeFallCharacter::RestoreAnimation()
{
	bBlockNewAnimation = false;
	if(!QueuedAnimation)
	{
		QueuedAnimation = DefaultAnimation;
	}
	PlayAnimation(QueuedAnimation, QueuedAnimationLooping);
}

#pragma endregion