// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CloudPlatform.generated.h"

UCLASS()
class ACloudPlatform : public APawn
{
	GENERATED_BODY()
protected:

	// Sets default values for this pawn's properties
	ACloudPlatform();


	/**Collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collider, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionComp;

	/**Set an arrow component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collider, meta = (AllowPrivateAccess = "true"))
		class UArrowComponent* ArrowComponent;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class UProjectileMovementComponent* ProjectileMovement;

	//A storage for the desired velocity of the platform after being summoned
	FVector Velocity;

public:

	//Add momentum to the cloud after spawn
	void AddMomentum(FVector FiringObjectVelocity);
};
