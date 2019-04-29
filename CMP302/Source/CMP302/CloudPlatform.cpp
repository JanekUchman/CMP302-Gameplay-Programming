// Fill out your copyright notice in the Description page of Project Settings.


#include "CloudPlatform.h"
#include <Components/PrimitiveComponent.h>
#include <Components/BoxComponent.h>
#include <Components/ArrowComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>
#include "TimerManager.h"

// Sets default values
ACloudPlatform::ACloudPlatform()
{
	//Box collider for the player to stand on 
	CollisionComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
	//Set an arrow as the root scene object so the collider can be scaled and rotated directly
	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Root Scene"));
	
	
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	CollisionComp->CanCharacterStepUpOn = ECB_Yes;
	//We want the cloud to appear ethereal and fly through objects
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 0.f;
	ProjectileMovement->MaxSpeed = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0;

}

void ACloudPlatform::AddMomentum(FVector FiringObjectVelocity)
{
	//Wait a tick then add the player's momentum onto the cloud after summon
	//Allows the cloud to follow the player's movement 
	Velocity = FiringObjectVelocity;
	FTimerHandle    handle;
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]() 
		{
		ProjectileMovement->Velocity = Velocity;
		});
}


