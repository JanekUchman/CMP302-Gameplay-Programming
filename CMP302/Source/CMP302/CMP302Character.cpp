// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CMP302Character.h"
#include "CMP302Projectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
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

	//The location where staff launches will originate from, around head height
	StaffPosition = FVector(0.0f, 0.0f, 60.0f);

	//The offset for launching the staff projectile, to the side so it feels as if it's been thrown
	StaffOffset = FVector(40, 0, 0);

	//The location of the cloud spawning in relation to our character, spawned beneath feet
	CloudOffset = FVector(0, 0, -60);

	//Set bool defaults
	bCanStaffJump = true;
	bStaffThrown = false;

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
	//Check if there's already a staff thrown, and if we're allowed to throw the staff
	if (IsValid(SpawnedStaff) || StaffClass == NULL || World == NULL || !bCanThrowStaff) return;

	const FRotator SpawnRotation = GetControlRotation();
	//Get the world space of the staff offset using the character's location
	const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(StaffOffset);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnedStaff = Cast<AStaffProjectile>(World->SpawnActor(StaffClass, &SpawnLocation, &SpawnRotation, SpawnParams));
	//Add momentum in the direction the player is moving
	if (SpawnedStaff) SpawnedStaff->AddMomentum(Movement->Velocity);
}

void AWukong::OnStaffCallBack()
{
	//Check we have something to call back
	if (SpawnedStaff == NULL ) return;
	//Notify the staff it's being returned
	SpawnedStaff->OnStaffCallBack();
}

//Wrappers for the input binding events
void AWukong::OnStaffForwards()
{
	LaunchOffStaff(FVector(1, 1, 1));
}

void AWukong::OnStaffBackwards()
{
	LaunchOffStaff(FVector(-1, -1, -1));
}

void AWukong::LaunchOffStaff(FVector DirectionModifier) const
{
	
	UWorld* const World = GetWorld();
	//Check if we can jump
	if (World == NULL || !bCanStaffJump) return;
	//Get the direction of the player, rotated towards the direction of the jump
	const FVector Direction = GetControlRotation().Vector() * DirectionModifier;
	//World location of the staff spawn
	const FVector StartLocation = GetActorLocation() + StaffPosition;
	//Multiply along the rotation of the player using the world position to find where the staff ends
	const FVector EndLocation = Direction * StaffLength + GetActorLocation() + StaffPosition;

	//Ignore all objects but static and dynamic types
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjects;
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));
	TraceObjects.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	//A result to store the hit into
	FHitResult FirstHit(ForceInit);
	//We don't care to ignore any actors
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(NULL);

	//Get our hit, if there's nothing return
	if (!UKismetSystemLibrary::LineTraceSingleForObjects(World, StartLocation, EndLocation, TraceObjects, 
		false, ActorsToIgnore, EDrawDebugTrace::None, FirstHit, true)) return;

	//Get the angle between the point of impact and the starting location
	FVector impulseAngle = StartLocation -FirstHit.ImpactPoint;
	impulseAngle.Normalize();
	//Launch the player based on the distance they were from the point of impact, the further away the weaker the force
	FVector impulse = LaunchVelocity * impulseAngle / (FirstHit.Distance / StaffLength);
	Movement->AddImpulse(impulse, true);
}


void AWukong::OnCloud()
{
	UWorld* const World = GetWorld();
	if (CloudPlatformClass == NULL || World == NULL || !bCanCloudRide) return;

	// Get the point at the player's feet
	const FVector SpawnLocation = GetActorLocation() + CloudOffset;

	//Spawn a cloud and set it to move along the player's current trajectory
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
