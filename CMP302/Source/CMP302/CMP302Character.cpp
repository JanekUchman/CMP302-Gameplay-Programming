// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CMP302Character.h"
#include "CMP302Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "CloudPlatform.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACMP302Character
AWukong::AWukong()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Default offset from the character location for projectiles to spawn
	StaffPosition = FVector(0.0f, 0.0f, 60.0f);

	StaffOffset = FVector(40, 0, 0);

	CloudOffset = FVector(0, 0, -60);

	CanStaffJump = true;

}

void AWukong::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	Movement = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetCharacterMovement();

}

//////////////////////////////////////////////////////////////////////////
// Input

void AWukong::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind throw event
	PlayerInputComponent->BindAction("Fire Staff", IE_Pressed, this, &AWukong::OnThrow);
	PlayerInputComponent->BindAction("Call Back Staff", IE_Pressed, this, &AWukong::OnStaffCallBack);

	//Bind launch events
	PlayerInputComponent->BindAction("Staff Backwards", IE_Pressed, this, &AWukong::OnStaffBackwards);
	PlayerInputComponent->BindAction("Staff Forwards", IE_Pressed, this, &AWukong::OnStaffForwards);

	//Bind cloud
	PlayerInputComponent->BindAction("Cloud", IE_Pressed, this, &AWukong::OnCloud);

	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AWukong::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AWukong::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AWukong::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AWukong::LookUpAtRate);

}

void AWukong::OnThrow()
{
	UWorld* const World = GetWorld();
	if (IsValid(SpawnedStaff) || StaffClass == NULL || World == NULL) return;

	// try and fire a projectile
	
	const FRotator SpawnRotation = GetControlRotation();
	//// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(StaffOffset);
	SpawnedStaff = Cast<AStaffProjectile>(World->SpawnActor(StaffClass, &SpawnLocation, &SpawnRotation));
	SpawnedStaff->AddMomentum(Movement->Velocity);
}

void AWukong::OnStaffCallBack()
{
	if (SpawnedStaff == NULL) return;

	SpawnedStaff->OnStaffCallBack();
}

void AWukong::OnStaffForwards()
{
	LaunchOffStaff(FVector(1, 1, 1));
}

void AWukong::OnStaffBackwards()
{
	LaunchOffStaff(FVector(-1, -1, -1));
}

void AWukong::LaunchOffStaff(FVector DirectionModifier)
{
	
	UWorld* const World = GetWorld();
	if (World == NULL || !CanStaffJump) return;
	const FVector Direction = GetControlRotation().Vector() * DirectionModifier;

	const FVector StartLocation = GetActorLocation() + StaffPosition;

	const FVector EndLocation = Direction * StaffLength + GetActorLocation() + StaffPosition;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	FHitResult FirstHit(ForceInit);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(NULL);
	if (!UKismetSystemLibrary::LineTraceSingleForObjects(World, StartLocation, EndLocation, TraceObjects, 
		false, ActorsToIgnore, EDrawDebugTrace::Persistent, FirstHit, true)) return;


	FVector impulseAngle = StartLocation -FirstHit.ImpactPoint;
	impulseAngle.Normalize();
	FVector impulse = LaunchVelocity * impulseAngle / (FirstHit.Distance / StaffLength);
	Movement->AddImpulse(impulse);
}

//void AWukong::OnStaffBackwards()
//{
//
//	/*UWorld* const World = GetWorld();
//	if (World == NULL || !CanStaffJump) return;
//
//	const FRotator SpawnRotation = GetControlRotation();
//	FVector Direction = SpawnRotation.Vector();
//
//	const FVector StartLocation = GetActorLocation() + StaffPosition;
//
//	const FVector EndLocation = -Direction * StaffLength + GetActorLocation() + StaffPosition;
//
//	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
//	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
//	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
//	FHitResult FirstHit(ForceInit);
//	TArray<AActor*> ActorsToIgnore;
//	ActorsToIgnore.Add(NULL);
//	if (!UKismetSystemLibrary::LineTraceSingleForObjects(World, StartLocation, EndLocation, TraceObjects, 
//		false, ActorsToIgnore, EDrawDebugTrace::Persistent, FirstHit, true)) return;
//
//
//	FVector impulseAngle =  StartLocation-FirstHit.ImpactPoint;
//	impulseAngle.Normalize();
//	FVector impulse = LaunchVelocity * impulseAngle / (FirstHit.Distance/ StaffLength);
//	Movement->AddImpulse(impulse);*/
//	
//}


void AWukong::OnCloud()
{
	UWorld* const World = GetWorld();
	if (CloudPlatformClass == NULL || World == NULL) return;

	// try and fire a projectile
	const FVector SpawnLocation = GetActorLocation() + CloudOffset;

	SpawnedCloud = Cast<ACloudPlatform>(World->SpawnActor(CloudPlatformClass, &SpawnLocation));
	SpawnedCloud->AddMomentum(Movement->Velocity);
}


void AWukong::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AWukong::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AWukong::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AWukong::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}