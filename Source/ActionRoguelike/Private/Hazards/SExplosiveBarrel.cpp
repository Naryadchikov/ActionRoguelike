// Fill out your copyright notice in the Description page of Project Settings.


#include "Hazards/SExplosiveBarrel.h"

#include "SMagicProjectile.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	// Set up mesh component
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	StaticMeshComp->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	StaticMeshComp->SetSimulatePhysics(true);
	RootComponent = StaticMeshComp;

	// Set up radial force component
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->Radius = 700.0f;
	RadialForceComp->ImpulseStrength = 1000.0f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->SetAutoActivate(false);
	RadialForceComp->SetupAttachment(StaticMeshComp);
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// More consistent to bind here compared to Constructor which may fail to bind if Blueprint was created before adding this binding (or when using hot reload)
	// PostInitializeComponent is the preferred way of binding any events.
	StaticMeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("OnActorHit in Explosive Barrel"));

	if (OtherActor->IsA(ASMagicProjectile::StaticClass()))
	{
		RadialForceComp->FireImpulse();

		UE_LOG(LogTemp, Log, TEXT("Projectile %s hit ExplosiveBarrel %s at game time: %f"),
		       *GetNameSafe(OtherActor), *GetNameSafe(this), GetWorld()->TimeSeconds);
	}
	else
	{
		UE_LOG(LogTemp, Log,
		       TEXT("ExplosiveBarrel %s was hit by another object %s which is not a 'Projectile' at game time: %f"),
		       *GetNameSafe(this), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);
	}
}
