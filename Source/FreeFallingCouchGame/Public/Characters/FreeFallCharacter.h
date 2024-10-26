// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FreeFallCharacterGrabbingState"
#include "GameFramework/Character.h"
#include "FreeFallCharacter.generated.h"

class AParachute;
enum class EDiveLayersID : uint8;
class ADiveLevels;
struct FInputActionValue;
class UInputMappingContext;
class UFreeFallCharacterInputData;
class UFreeFallCharacterState;
enum class EFreeFallCharacterStateID : uint8;
class UFreeFallCharacterStateMachine;

UCLASS()
class FREEFALLINGCOUCHGAME_API AFreeFallCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Unreal Default
public:
	// Sets default values for this character's properties
	AFreeFallCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

#pragma endregion

public:
	void DestroyPlayer();
	
#pragma region StateMachine
public:
	void CreateStateMachine();

	void InitStateMachine();

	void TickStateMachine(float DeltaTime) const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFreeFallCharacterStateMachine> StateMachine;
	
public:
	TObjectPtr<UFreeFallCharacterStateMachine> GetStateMachine() const;
	
	UPROPERTY(EditAnywhere)
	TMap<EFreeFallCharacterStateID, TSubclassOf<UFreeFallCharacterState>> FreeFallCharacterStatesOverride;

#pragma endregion

#pragma region Input Data / Mapping Context

public:
	UPROPERTY()
	TObjectPtr<UFreeFallCharacterInputData> InputData;

	UPROPERTY()
	TObjectPtr<UInputMappingContext> InputMappingContext;

protected:
	void SetupMappingContextIntoController() const;

#pragma endregion

#pragma region Input Move

public:
	FVector2D GetInputMove() const;

protected:
	UPROPERTY()
	FVector2D InputMove = FVector2D::ZeroVector;
private:
	void BindInputMoveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputMove(const FInputActionValue& Value);
	
#pragma endregion

#pragma region Input Dive

public:
	float GetInputDive() const;

	UFUNCTION(BlueprintCallable)
	void SetDiveMaterialColor();
	
	ADiveLevels* GetDiveLevelsActor() const;
	float GetDiveLayerForceStrength() const;
	ACameraActor* GetCameraActor() const;

	UPROPERTY(EditAnywhere)
	TMap<EDiveLayersID, FLinearColor> DiveLevelsColors;

protected:
	UPROPERTY()
	float InputDive = 0.f;

	UPROPERTY(EditAnywhere)
	float DiveLayerForceStrength = 1.f;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* DiveMaterialInstance;

	UPROPERTY()
	ADiveLevels* DiveLevelsActor;

	UPROPERTY()
	ACameraActor* CameraActor;
	
private:
	void BindInputDiveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputDive(const FInputActionValue& Value);

#pragma endregion

#pragma region Input Grab

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputGrabbing);
	FInputGrabbing OnInputGrabEvent;
	
private:
	void BindInputGrabActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputGrab(const FInputActionValue& Value);

	UFUNCTION()
	void UpdateMovementInfluence(float DeltaTime) const;
	UFUNCTION()
	void UpdateObjectPosition(float DeltaTime) const;
	UFUNCTION()
	void UpdateHeavyObjectPosition(float DeltaTime);
	
public:
	bool bInputGrabPressed = false;
	EFreeFallCharacterGrabbingState GrabbingState;
	
	UPROPERTY()
	TObjectPtr<AFreeFallCharacter> OtherCharacter;

	UPROPERTY()
	TObjectPtr<AActor> OtherObject;

	FVector GrabHeavyObjectRelativeLocationPoint;
	
	//Fields are set by Grab State
	FVector GrabInitialOffset;
	float GrabRotationSpeed;
	float GrabRotationInfluenceStrength;
	FRotator GrabDefaultRotationOffset;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> ObjectGrabPoint;

public :
	TObjectPtr<USceneComponent> GetObjectGrabPoint() const;
	
#pragma endregion 

#pragma region IDPlayer
protected:
	uint8 ID_PlayerLinked = -1;
public:
	int getIDPlayerLinked() const { return ID_PlayerLinked; }
	void setIDPlayerLinked(int InID) { ID_PlayerLinked = InID; }
#pragma endregion

#pragma region Bounce Collision
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleCollision;

	UPROPERTY(BlueprintReadWrite)
	bool bAlreadyCollided = false;

	/*Cooldown entre 2 rebonds*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision")
	float BounceCooldownDelay = .1f;
	
	/*Combien JE donne à l'autre joueur / l'autre obstacle (je garde 1 - X)*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision")
	float BouncePlayerRestitutionMultiplier = 1.f;

	/*Dois-je garder ma vélocité restante ou non ?*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision")
	bool bShouldKeepRemainingVelocity = false;
	
	/*Multiplicateur de rebondissement entre les joueurs*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision - Between players")
	float BouncePlayerMultiplier = 1.f;

	/*Multiplicateur de rebondissement entre joueur / objets*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision - Player / Object")
	float BounceObstacleMultiplier = 1.f;

	/*Combien L'obstacle avec qui je collisionne donne à mon joueur (il garde 1 - X)*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision - Player / Object")
	float BounceObstacleRestitutionMultiplier = 1.f;

	/*Dois-je considerer la masse de l'objet dans les calcules de rebond ?
	 * Coté joueur -> ObstacleMass / PlayerMass
	 * Coté objet -> PlayerMass / ObstacleMass 
	 */
	UPROPERTY(EditAnywhere, Category="Bounce Collision - Player / Object")
	bool bShouldConsiderMassObject = false;
	
	/*La masse du joueur (sert pour les collisions entre objets)*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision - Player / Object")
	float PlayerMass;
	
	UPROPERTY()
	FVector OldVelocity = FVector::ZeroVector;

public:
	UFUNCTION(BlueprintCallable)
	void BounceCooldown();

	UFUNCTION()
	float GetPlayerMass();
	
protected:
	UFUNCTION()
	void OnCapsuleCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void ResetBounce();
	
#pragma endregion

#pragma region Parachute

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AParachute> Parachute;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> ParachuteAttachPoint;

public:
	UFUNCTION()
	USceneComponent* GetParachuteAttachPoint(); 
	
#pragma endregion
};
