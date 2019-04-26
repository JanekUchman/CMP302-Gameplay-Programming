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
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;



public:
	AWukong();

protected:
	virtual void BeginPlay();

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

	/** The offset for spawning the staff projectile. */
	UPROPERTY(EditAnywhere, Category = Cloud)
	FVector CloudOffset;

	/** Staff's offset from the characters location */
	UPROPERTY(EditAnywhere, Category=Staff)
	FVector StaffPosition;

	UPROPERTY(EditAnywhere, BluePrintReadWrite, Category = Staff)
		bool CanStaffJump;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Staff)
	TSubclassOf<class AStaffProjectile> StaffClass;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Cloud)
	TSubclassOf<class ACloudPlatform> CloudPlatformClass;




protected:
	
	/** Fires a projectile. */
	void OnThrow();

	void OnStaffBackwards();

	void OnStaffForwards();

	void OnStaffCallBack();

	void OnCloud();

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

	
	UCharacterMovementComponent* Movement;

	AStaffProjectile* SpawnedStaff;

	ACloudPlatform* SpawnedCloud;
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface


public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

