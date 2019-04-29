// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <GameFramework/Character.h>
#include "CMP302Projectile.generated.h"

UCLASS()
class AStaffProjectile : public APawn
{
	GENERATED_BODY()
protected:

	/** Collision component */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collider, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	/** The power with which the player will be launched into the air upon landing. */
	UPROPERTY(EditAnywhere, Category = LaunchSettings)
	float LaunchPower;

	//The overlap of the player and the staff
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector Velocity;
public:
	AStaffProjectile();

	//What happens when the staff is recalled
	virtual void OnStaffCallBack();

	void AddMomentum(FVector FiringObjectVelocity);
};

