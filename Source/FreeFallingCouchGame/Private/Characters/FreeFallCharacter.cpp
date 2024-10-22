// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/FreeFallCharacter.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Characters/FreeFallCharacterInputData.h"
#include "Characters/FreeFallCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
AFreeFallCharacter::AFreeFallCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AFreeFallCharacter::BeginPlay()
{
	Super::BeginPlay();
	CreateStateMachine();
	InitStateMachine();

	DefaultZPosition = GetActorLocation().Z;
	
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

// Called every frame
void AFreeFallCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickStateMachine(DeltaTime);
}

// Called to bind functionality to input
void AFreeFallCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	SetupMappingContextIntoController();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr) return;

	BindInputMoveAxisAndActions(EnhancedInputComponent);
	BindInputDiveAxisAndActions(EnhancedInputComponent);
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

FVector2D AFreeFallCharacter::GetInputMove() const
{
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

float AFreeFallCharacter::GetInputDive() const
{
	return InputDive;
}

float AFreeFallCharacter::GetDefaultZPosition() const
{
	return DefaultZPosition;
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

