// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CMP302Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include <Components/InputComponent.h>
#include <Components/BoxComponent.h>
#include <Components/CapsuleComponent.h>
#include "CMP302Character.h"

AStaffProjectile::AStaffProjectile() 
{
	// Use a sphere as a simple collision representation
	//CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Root Scene"));
	/*CollisionComp = NewObject<UCapsuleComponent>(this, FName("Capsule Collider"));
	CollisionComp->RegisterComponent();
	this->AddInstanceComponent(CollisionComp);*/
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// set up a notification for when this component hits something blocking
	CollisionComp->OnComponentHit.AddDynamic(this, &AStaffProjectile::OnHit);
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_Yes;
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// Set as root component
	//RootComponent = CollisionComp;
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 20000.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AStaffProjectile::BeginOverlap);

	LaunchPower = 1000;
}

void AStaffProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//// Only add impulse and destroy projectile if we hit a physics
	//if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	//{
	//	OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

	//	Destroy();
	//}
}

void AStaffProjectile::OnStaffCallBack()
{
	UE_LOG(LogTemp, Warning, TEXT("Your message"));
	Destroy();
}

void AStaffProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AWukong* wukong = Cast<AWukong>(OtherActor);
	if (wukong != nullptr)
	{
		const FVector LaunchVelocity = FVector(0, 0, LaunchPower);
		wukong->LaunchCharacter(LaunchVelocity, false, true);
	}
}

void AStaffProjectile::AddMomentum(FVector FiringObjectVelocity)
{
	ProjectileMovement->Velocity += FiringObjectVelocity;
}



