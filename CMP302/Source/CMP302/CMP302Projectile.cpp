// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CMP302Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Components/BoxComponent.h>
#include "CMP302Character.h"
#include "TimerManager.h"


AStaffProjectile::AStaffProjectile() 
{
	//Set a box collision as it's easy for the player to land on and most accuractely represents the staff
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Root Scene"));

	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	// Players can't walk on it as it's just going to bounce them upwards
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_Yes;
	//Block all channels but pawns, allow overlap to allow for character interaction
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	//We want the staff to fly accurately towards where the player's directing it
	ProjectileMovement->ProjectileGravityScale = 0;

	//Set up the collision function with the player
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AStaffProjectile::BeginOverlap);

	LaunchPower = 1000;
}

void AStaffProjectile::OnStaffCallBack()
{
	//Simply destroy the staff, given the appearance of recalling it
	Destroy();
}

void AStaffProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Check if the collision is with our player
	AWukong* wukong = Cast<AWukong>(OtherActor);
	if (wukong != nullptr)
	{
		//If it is launch them vertically, maintain their X, Y momentum however
		const FVector LaunchVelocity = FVector(0, 0, LaunchPower);
		wukong->LaunchCharacter(LaunchVelocity, false, true);
	}
}

void AStaffProjectile::AddMomentum(FVector FiringObjectVelocity)
{
	//Wait a tick then add the player's momentum onto the staff after throwing
	//Creates a more realistic experience and adds an extra layer of skill
	Velocity = FiringObjectVelocity;
	FTimerHandle    handle;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			ProjectileMovement->Velocity += Velocity;
		});
}



