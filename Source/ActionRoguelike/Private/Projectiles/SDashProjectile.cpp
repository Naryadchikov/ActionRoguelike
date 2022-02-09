// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectiles/SDashProjectile.h"

#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASDashProjectile::ASDashProjectile()
{
	// Set up root collision component
	SphereComp->SetSphereRadius(20.0f);
	SphereComp->OnComponentHit.AddDynamic(this, &ASDashProjectile::OnDashProjectileHit);

	// Set up explosion effect component
	ExplosionEffectComp = CreateDefaultSubobject<UParticleSystemComponent>("ExplosionEffectComp");
	ExplosionEffectComp->SetAutoActivate(false);
	ExplosionEffectComp->SetupAttachment(RootComponent);

	// Set up timers' delay
	ExplosionDelay = 0.2f;
	TeleportationDelay = 0.2f;
}

// Called when the game starts or when spawned
void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();


	// Set explosion after specified time
	const FTimerDelegate TimerDelegate_Explosion = FTimerDelegate::CreateUObject(
		this, &ASDashProjectile::Explode, false, FVector::ZeroVector);

	GetWorldTimerManager().SetTimer(TimerHandle_Explosion, TimerDelegate_Explosion, ExplosionDelay, false);
}

void ASDashProjectile::Explode(bool bHit, FVector HitLocation)
{
	// Stop projectile movement
	MovementComp->StopMovementImmediately();

	// Deactivate primary projectile effect
	EffectComp->DeactivateSystem();

	// Turn off all collisions
	SetActorEnableCollision(false);

	// Let the detonation effect play before teleportation
	if (ExplosionEffectComp->Template)
	{
		ExplosionEffectComp->Activate();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Particle System is not set in ExplosionEffectComp."));
	}

	// Teleport player after specified time
	const FVector TeleportLocation = bHit ? HitLocation : GetActorLocation();
	const FTimerDelegate TimerDelegate_Teleportation = FTimerDelegate::CreateUObject(
		this, &ASDashProjectile::TeleportInstigator, TeleportLocation);

	GetWorldTimerManager().SetTimer(TimerHandle_Teleportation, TimerDelegate_Teleportation, TeleportationDelay, false);
}

void ASDashProjectile::TeleportInstigator(FVector TeleportLocation)
{
	FRotator TeleportRotation = GetActorRotation();
	TeleportRotation.Pitch = 0.0f;
	TeleportRotation.Roll = 0.0f;

	GetInstigator()->TeleportTo(TeleportLocation, TeleportRotation);

	Destroy();
}

void ASDashProjectile::OnDashProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (GetWorldTimerManager().IsTimerActive(TimerHandle_Explosion))
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Explosion);
		Explode(true, Hit.ImpactPoint);
	}
}

void ASDashProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clear All timers that belong to this actor.
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}
