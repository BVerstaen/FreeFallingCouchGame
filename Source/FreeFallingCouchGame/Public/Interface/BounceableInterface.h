// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BounceableInterface.generated.h"

class AFreeFallCharacter;

UENUM()
enum EBounceParameters
{
	None = 0,
	Obstacle,
	HeavyObstacle,
	Player
};

USTRUCT()
struct FBounceData
{
	GENERATED_BODY()
	
public:
	/*Combien JE donne à l'autre joueur / l'autre obstacle (je garde 1 - X)*/
	UPROPERTY(EditAnywhere)
	float SelfRestitutionMultiplier = 1.f;

	/*Combien L'obstacle avec qui je collisionne donne à mon joueur (il garde 1 - X)*/
	UPROPERTY(EditAnywhere)
	float OtherRestitutionMultiplier = 1.f;

	/*Multiplicateur de rebondissement*/
	UPROPERTY(EditAnywhere)
	float BounceMultiplier = 1.f;

	/*Dois-je considerer la masse de l'objet dans les calcules de rebond ?
	* Coté joueur -> ObstacleMass / PlayerMass
	* Coté objet -> PlayerMass / ObstacleMass 
	*/
	UPROPERTY(EditAnywhere)
	bool bShouldConsiderMass = false;
	
	/*Dois-je garder ma vélocité restante ou non ?*/
	UPROPERTY(EditAnywhere)
	bool bShouldKeepRemainingVelocity = false;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UBounceableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FREEFALLINGCOUCHGAME_API IBounceableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FVector GetVelocity() = 0;
	virtual float GetMass() = 0;
	virtual EBounceParameters GetBounceParameterType() = 0;
	
	virtual void AddBounceForce(FVector Velocity) = 0;

	virtual AFreeFallCharacter* CollidedWithPlayer() = 0;
};
