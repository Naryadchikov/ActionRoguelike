// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectileBase.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASProjectileBase::ASProjectileBase()
{
	// Set up collision component
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	RootComponent = SphereComp;

	// Set up projectile effect component
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(RootComponent);

	// Set up projectile movement component
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->ProjectileGravityScale = 0.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
}

void ASProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Set ignore instigator collision
	if (APawn* ProjectileInstigator = GetInstigator())
	{
		SphereComp->IgnoreActorWhenMoving(ProjectileInstigator, true);
		ProjectileInstigator->MoveIgnoreActorAdd(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile %s does not have Instigator."), *GetNameSafe(this));
	}
}

// Called when the game starts or when spawned
void ASProjectileBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
