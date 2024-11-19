﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FreeFallCharacterGrabbingState"
#include "GameFramework/Character.h"
#include "Interface/DiveLayersSensible.h"
#include "Other/DiveLayersID.h"
#include "Interface/BounceableInterface.h"
#include "NiagaraSystem.h"
#include "FreeFallCharacter.generated.h"

class UCharactersSettings;
class UPowerUpObject;
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
class FREEFALLINGCOUCHGAME_API AFreeFallCharacter : public ACharacter, public IDiveLayersSensible, public IBounceableInterface
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

#pragma region Mesh Rotation
	
public :
	void InterpMeshPlayer(FRotator Destination, float DeltaTime, float DampingSpeed);
	FRotator GetPlayerDefaultRotation();

private:
	FRotator PlayerMeshDefaultRotation;

#pragma endregion
	
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

#pragma region Move

public:
	UPROPERTY()
	FVector2D AccelerationAlpha;

	UPROPERTY()
	bool bShouldOrientToMovement;

	UPROPERTY(EditAnywhere, Category = "Horizontal Movement")
	float MaxAccelerationValue;

	UPROPERTY(EditAnywhere, Category = "Horizontal Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, Category = "Horizontal Movement")
	float DecelerationSpeed;

protected:
	UPROPERTY()
	const UCharactersSettings* CharactersSettings;
	
	/*Le seuil à partir duquel le joueur ne bloque plus sa rotation et permet d'être influencé (uniquement si attrape joueur)*/
	UPROPERTY(EditAnywhere, Category="Grab Threshold")
	float OrientationThreshold;
	
	/*Le seuil à partir duquel le joueur ne bloque plus sa rotation et permet d'être influencé (uniquement si attrape joueur)*/
	UPROPERTY(EditAnywhere, Category="Grab Threshold")
	float GrabbedOrientationThreshold;

	/*Le seuil à partir duquel le joueur ne bloque plus sa rotation de risque qu'il colissionne le joueur grab avec le joueur grabbé (uniquement si attrape et attrapée)*/
	UPROPERTY(EditAnywhere, Category="Grab Threshold")
	float GrabToCloseToGrabbedAngle;

private:
	void ApplyMovementFromAcceleration(float DeltaTime);

	void Decelerate(float DeltaTime);

	FVector2D GrabOldInputDirection;

#pragma region Mesh movement
	
protected:
	/*Rotation maximum en Yaw du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementRotationAngle;
	
	/*Vitesse de rotation du joueur lorsqu'il se déplace*/
	UPROPERTY(EditAnywhere, Category="Mesh movement")
	float MeshMovementDampingSpeed;

	FVector2D PreviousInputMovement;
	FRotator PreviousRotation = FRotator::ZeroRotator;
	
#pragma endregion
	
#pragma endregion

#pragma region Input Dive

public:
	float GetInputDive();

	UFUNCTION(BlueprintCallable)
	void SetDiveMaterialColor();
	
	ADiveLevels* GetDiveLevelsActor() const;
	ACameraActor* GetCameraActor() const;

	UPROPERTY(EditAnywhere)
	TMap<EDiveLayersID, FLinearColor> DiveLevelsColors;

	bool InvertDiveInput = false;
protected:
	UPROPERTY()
	float InputDive = 0.f;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> DiveMaterialInstance;

	UPROPERTY()
	TObjectPtr<ADiveLevels> DiveLevelsActor;

	UPROPERTY()
	TObjectPtr<ACameraActor> CameraActor;
	
private:
	void BindInputDiveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputDive(const FInputActionValue& Value);

#pragma endregion

#pragma region Input FastDive

public:
	float GetInputFastDive();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputFastDive);
	FInputFastDive OnInputFastDiveEvent;

protected:
	UPROPERTY()
	float InputFastDive = 0.f;
	
private:
	void BindInputFastDiveAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputFastDive(const FInputActionValue& Value);

#pragma endregion

#pragma region DiveLayerSensible Interface

public:
	virtual void ApplyDiveForce(FVector DiveForceDirection, float DiveStrength) override;

	virtual EDiveLayersID GetBoundedLayer() override;

	virtual AActor* GetSelfActor() override;

	virtual FVector GetDivingVelocity() override;
	
	virtual bool IsBoundedByLayer() override;

	virtual bool IsDiveForced() override;
	
	UPROPERTY()
	bool bIsBoundedByLayer = true;

	UPROPERTY()
	bool bIsDiveForced = true;

	UPROPERTY()
	EDiveLayersID BoundedLayer = EDiveLayersID::None;

	UPROPERTY()
	FVector DivingVelocity = FVector::ZeroVector;
	
#pragma endregion

#pragma region Input Grab

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputGrabbing);
	FInputGrabbing OnInputGrabEvent;
	
private:
	void BindInputGrabActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputGrab(const FInputActionValue& Value);
	
	bool IsInCircularGrab();

	//Update one movement influence with another character
	UFUNCTION()
	void UpdateMovementInfluence(float DeltaTime, AFreeFallCharacter* OtherCharacter, bool bIsCircularGrab);
	//Check and update every movement influence
	UFUNCTION()
	void UpdateEveryMovementInfluence(float DeltaTime);
	//Update smaller object influence
	UFUNCTION()
	void UpdateObjectPosition(float DeltaTime) const;
	//Update heavier object influence
	UFUNCTION()
	void UpdateHeavyObjectPosition(float DeltaTime);
	//Check and update if there's any dissociation problem -> handy to avoid physics problem
	UFUNCTION()
	void UpdateDissociationProblems(float DeltaTime);
	
public:
	//Look if looking to close to the player who grabs me -> handy to avoid physics problem 
	UFUNCTION()
	bool IsLookingToCloseToGrabber(float AngleLimit);
	
public:
	bool bInputGrabPressed = false;
	EFreeFallCharacterGrabbingState GrabbingState;

	//The one I grabbed
	UPROPERTY()
	TObjectPtr<AFreeFallCharacter> OtherCharacterGrabbing;

	//The one who's grabbing me
	UPROPERTY()
	TObjectPtr<AFreeFallCharacter> OtherCharacterGrabbedBy;
	
	UPROPERTY()
	TObjectPtr<AActor> OtherObject;

	UPROPERTY()	
	bool bIsGrabbable = true;
	
	FVector GrabHeavyObjectRelativeLocationPoint;
	
	//Fields are set by Grab State
	FVector GrabInitialOffset;
	float GrabRotationSpeed;
	float GrabRotationInfluenceStrength;
	FRotator GrabDefaultRotationOffset;
	FRotator GrabCircularRotationOffset;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> ObjectGrabPoint;

public :
	TObjectPtr<USceneComponent> GetObjectGrabPoint() const;
	
#pragma endregion 

#pragma region Input DeGrab

private:
	void BindInputDeGrabActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputDeGrab(const FInputActionValue& Value);

	/*Nombre maximum d'input degrab à appuyer pour se libérer*/
	UPROPERTY(EditAnywhere, Category="DeGrab")
	int MaxNumberOfDeGrabInput = 10;
	
	UPROPERTY()
	int CurrentNumberOfDeGrabInput = 0;

public:
	UFUNCTION()
	void ActivateDeGrab();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateEffectDeGrab();

	UFUNCTION(BlueprintImplementableEvent)
	void StopEffectDeGrab();

#pragma endregion 
	
#pragma region IDPlayer
protected:
	uint8 ID_PlayerLinked = -1;
public:
	UFUNCTION(BlueprintCallable)
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

public:
	/*La masse du joueur (sert pour les collisions entre objets)*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision - Player / Object")
	float PlayerMass;

protected:
	UPROPERTY()
	bool bWasRecentlyBounced;

	/*
	 *		Eliminations Properties
	 */
	
	/*A partir de combien de temps un rebond qui mène à un OUT ne compte plus comme une élimination ?*/
	UPROPERTY(EditAnywhere, Category="Bounce Collision - Elimination")
	float DelayConsideredAsRecentlyBounced;
	
	UPROPERTY()
	AFreeFallCharacter* RecentlyBouncedOtherPlayer;

	UPROPERTY()
	FTimerHandle RecentlyBouncedTimer; 

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWasEliminated, AFreeFallCharacter*, PreviousOwner, AFreeFallCharacter*, NextOwner);
	FWasEliminated OnWasEliminated;
	
public:
	UFUNCTION(BlueprintCallable)
	void BounceCooldown();

	UFUNCTION()
	void SetWasRecentlyBouncedTimer(AFreeFallCharacter* Character);
	
protected:
	UFUNCTION()
	void OnCapsuleCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
	UFUNCTION()
	void ResetBounce();

	UFUNCTION()
	void ResetWasRecentlyBounced();


public:
	virtual float GetMass() override;
	virtual FVector GetVelocity() override;
	virtual EBounceParameters GetBounceParameterType() override;
	virtual void AddBounceForce(FVector Velocity) override;
	virtual AFreeFallCharacter* CollidedWithPlayer() override;
	
	UFUNCTION()
	void BounceRoutine(AActor* OtherActor, TScriptInterface<IBounceableInterface> OtherBounceableInterface, float SelfRestitutionMultiplier, float OtherRestitutionMultiplier, float GlobalMultiplier, bool bShouldConsiderMass, bool bShouldKeepRemainVelocity);

protected:
	UPROPERTY(EditAnywhere, Category="Bounce Collision")
	TSoftObjectPtr<UNiagaraSystem> BounceEffect;
	
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

#pragma region PowerUp

public:
	UPROPERTY()
	TObjectPtr<UPowerUpObject> CurrentPowerUp;

	UPROPERTY()
	TArray<TObjectPtr<UPowerUpObject>> UsedPowerUps;
	
	UPROPERTY()
	bool bInputUsePowerUpPressed = false;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInputUsePowerUp);
	FInputUsePowerUp OnInputUsePowerUpEvent;
	
	void SetPowerUp(UPowerUpObject* PowerUpObject);
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTakePowerUp,const AFreeFallCharacter*, Character);
	FTakePowerUp OnTakePowerUp;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUsePowerUp,const AFreeFallCharacter*, Character);
	FUsePowerUp OnUsePowerUp;

private:
	void BindInputUsePowerUpActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputUsePowerUp(const FInputActionValue& Value);
	
#pragma endregion
};
