// Fill out your copyright notice in the Description page of Project Settings.


#include "CloudPlatform.h"
#include <Components/PrimitiveComponent.h>
#include <Components/BoxComponent.h>
#include <Components/ArrowComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

// Sets default values
ACloudPlatform::ACloudPlatform()
{
	// Use a sphere as a simple collision representation
	
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Root Scene"));
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_Yes;
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	// Set as root component
	//RootComponent = CollisionComp;
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1.f;
	ProjectileMovement->MaxSpeed = 20000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;

}

void ACloudPlatform::AddMomentum(FVector FiringObjectVelocity)
{
	ProjectileMovement->Velocity += FiringObjectVelocity;
}


