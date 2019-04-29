// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CMP302Projectile.h"
#include "CMP302Character.generated.h"

class UInputComponent;

UCLASS(config = Game, meta = (ShowOnlyInnerProperties))

class AWukong : public ACharacter
{
	GENERATED_BODY()
protected:

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
public:
	AWukong();

protected:
	virtual void BeginPlay() override;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere,  Category=Camera)
	float BaseLookUpRate;

	/** The velocity to launch the character using the staff. */
	UPROPERTY(EditAnywhere, Category = Staff)
	float LaunchVelocity;

	/** The length of the staff to search for surfaces. */
	UPROPERTY(EditAnywhere, Category = Staff)
	float StaffLength;

	/** The offset for spawning the staff projectile. */
	UPROPERTY(EditAnywhere, Category = Staff)
	FVector StaffOffset;

	/** The offset for spawning the cloud projectile. */
	UPROPERTY(EditAnywhere, Category = Cloud)
	FVector CloudOffset;

	/** Launching staff offset from the characters location */
	UPROPERTY(EditAnywhere, Category=Staff)
	FVector StaffPosition;

	/**Whether the staff has been throw or not*/
	UPROPERTY(EditAnywhere, Category = Staff)
	bool bStaffThrown;

	/**Can the player jump currently using the staff*/
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Staff)
	bool bCanStaffJump;

	/**Can the cloud ride*/
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Cloud)
	bool bCanCloudRide;

	/**Can the player throw the staff*/
	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Staff)
	bool bCanThrowStaff;

	/** Staff projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Staff)
	TSubclassOf<class AStaffProjectile> StaffClass;

	/** Cloud projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Cloud)
	TSubclassOf<class ACloudPlatform> CloudPlatformClass;




protected:
	//////////////////
	//Input
	virtual void OnThrow();
	virtual void OnStaffBackwards();
	virtual void OnStaffForwards();
	virtual void OnStaffCallBack();
	virtual void LaunchOffStaff(FVector DirectionModifier) const;
	virtual void OnCloud();
	//////////////////

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	//A reference to our character's movement as we use it frequently
	UCharacterMovementComponent* Movement;

	//A reference to the spawned staff projectile as we need to notify it of input
	AStaffProjectile* SpawnedStaff;

	//A reference to the spawned cloud
	ACloudPlatform* SpawnedCloud;
protected:
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
};

