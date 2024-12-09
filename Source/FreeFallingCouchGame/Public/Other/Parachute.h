// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/GrabbableInterface.h"
#include "Parachute.generated.h"

UCLASS()
class FREEFALLINGCOUCHGAME_API AParachute : public AActor, public IGrabbableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AParachute();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere)
	float LaunchForce;
	
	FVector StartingLocation;

	FVector OriginScale = FVector(1, 1, 0);
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual bool CanBeTaken() override;
	virtual bool CanBeGrabbed() override;

	UFUNCTION()
	virtual void EquipToPlayer(AFreeFallCharacter* Character);
	
	UFUNCTION()
	virtual void Use(AFreeFallCharacter* Character) override;

	UFUNCTION()
	void StealParachute(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NextOwner);
	
	//Give back reference -> if need it to launch Parachute
	UFUNCTION()
	AParachute* DropParachute(AFreeFallCharacter* PreviousOwner);

	UE_DEPRECATED(5.3, "Function is depreciated, no need to RecenterParachute anymore")
	void RecenterParachute() const;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFallDownAnimation(FVector EndLocation);
	
	UFUNCTION(BlueprintImplementableEvent)
	void StopFallDownAnimation();
	
private:
	UFUNCTION()
	void GiveToMurderer(AFreeFallCharacter* PreviousOwner, AFreeFallCharacter* NextOwner);

	/*
	 *Delegates
	 */
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParachuteGrabbed, AFreeFallCharacter*, Character);
	UPROPERTY(BlueprintAssignable)
	FOnParachuteGrabbed OnParachuteGrabbed;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnParachuteStolen, AFreeFallCharacter*, PreviousOwner, AFreeFallCharacter*, NextOwner);
	UPROPERTY(BlueprintAssignable)
	FOnParachuteStolen OnParachuteStolen;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParachuteDropped, AFreeFallCharacter*, PreviousOwner);
	UPROPERTY(BlueprintAssignable)
	FOnParachuteGrabbed OnParachuteDropped;
};
