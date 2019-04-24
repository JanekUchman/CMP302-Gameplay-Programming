// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CMP302Character.h"
#include "CMP302Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ACMP302Character

ACMP302Character::ACMP302Character()
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

	//// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	//Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	//Mesh1P->SetOnlyOwnerSee(true);
	//Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	//Mesh1P->bCastDynamicShadow = false;
	//Mesh1P->CastShadow = false;
	//Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	//Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	//// Create a gun mesh component
	//FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	//FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	//FP_Gun->bCastDynamicShadow = false;
	//FP_Gun->CastShadow = false;
	//// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	//FP_Gun->SetupAttachment(RootComponent);


	// Default offset from the character location for projectiles to spawn
	StaffPosition = FVector(0.0f, 0.0f, 60.0f);


}

void ACMP302Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	Movement = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)->GetCharacterMovement();

}

//////////////////////////////////////////////////////////////////////////
// Input

void ACMP302Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind throw event
	PlayerInputComponent->BindAction("Fire Staff", IE_Pressed, this, &ACMP302Character::OnThrow);
	PlayerInputComponent->BindAction("Call Back Staff", IE_Pressed, this, &ACMP302Character::OnStaffCallBack);

	//Bind launch events
	PlayerInputComponent->BindAction("Staff Backwards", IE_Pressed, this, &ACMP302Character::OnStaffBackwards);
	PlayerInputComponent->BindAction("Staff Forwards", IE_Pressed, this, &ACMP302Character::OnStaffForwards);

	//Bind cloud
	PlayerInputComponent->BindAction("Cloud", IE_Pressed, this, &ACMP302Character::OnCloud);

	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACMP302Character::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACMP302Character::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACMP302Character::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACMP302Character::LookUpAtRate);
}

void ACMP302Character::OnThrow()
{
	UWorld* const World = GetWorld();
	UE_LOG(LogTemp, Warning, TEXT("Test"));
	if (IsValid(SpawnedProjectile) || ProjectileClass == NULL || World == NULL) return;

	// try and fire a projectile
	
	const FRotator SpawnRotation = GetControlRotation();
	//// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
	const FVector SpawnLocation = GetActorLocation()  + SpawnRotation.RotateVector(StaffOffset);
	SpawnedProjectile = Cast<ACMP302Projectile>(World->SpawnActor(ProjectileClass, &SpawnLocation, &SpawnRotation));
	SpawnedProjectile->GetRootComponent()->ComponentVelocity = SpawnedProjectile->GetRootComponent()->ComponentVelocity + Movement->Velocity;
	////Set Spawn Collision Handling Override
	//FActorSpawnParameters ActorSpawnParams;
	//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

	//// spawn the projectile at the muzzle
	//World->SpawnActor<ACMP302Projectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);

}

void ACMP302Character::OnStaffForwards()
{
	
	UWorld* const World = GetWorld();
	if (World == NULL) return;
	const FVector Direction = GetControlRotation().Vector();

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

void ACMP302Character::OnStaffBackwards()
{

	UWorld* const World = GetWorld();
	if (World == NULL) return;

	const FRotator SpawnRotation = GetControlRotation();
	FVector Direction = SpawnRotation.Vector();

	const FVector StartLocation = GetActorLocation() + StaffPosition;

	const FVector EndLocation = -Direction * StaffLength + GetActorLocation() + StaffPosition;

	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	FHitResult FirstHit(ForceInit);
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(NULL);
	if (!UKismetSystemLibrary::LineTraceSingleForObjects(World, StartLocation, EndLocation, TraceObjects, 
		false, ActorsToIgnore, EDrawDebugTrace::Persistent, FirstHit, true)) return;


	FVector impulseAngle =  StartLocation-FirstHit.ImpactPoint;
	impulseAngle.Normalize();
	FVector impulse = LaunchVelocity * impulseAngle / (FirstHit.Distance/ StaffLength);
	Movement->AddImpulse(impulse);
	
}

void ACMP302Character::OnStaffCallBack()
{
}

void ACMP302Character::OnCloud()
{
}


void ACMP302Character::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACMP302Character::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACMP302Character::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACMP302Character::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}