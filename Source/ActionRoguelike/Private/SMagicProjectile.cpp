// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SGameplayFunctionLibrary.h"
#include "Components/SActionComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ASMagicProjectile::ASMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);

	ProjectileDamage = 20.0f;
}

void ASMagicProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
}

void ASMagicProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation(), GetActorRotation(), 1, 1, 0,
		                                      ImpactSoundAttenuation ? ImpactSoundAttenuation : nullptr);
	}

	if (ImpactEmitterTemplate)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEmitterTemplate, GetActorLocation());
	}

	if (WorldCameraShakeClass)
	{
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), WorldCameraShakeClass, Hit.Location, 0.0f, 1000.0f);
	}

	Destroy();
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator())
	{
		const USActionComponent* ActionComp = Cast<USActionComponent>(
			OtherActor->GetComponentByClass(USActionComponent::StaticClass()));

		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = -MovementComp->Velocity;
			SetInstigator(Cast<APawn>(OtherActor));

			return;
		}

		if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, ProjectileDamage,
		                                                      SweepResult))
		{
			// Calling the same logic as on hit for spawning effects and destroying the projectile
			OnProjectileHit(OverlappedComponent, OtherActor, OtherComp, SweepResult.ImpactNormal, SweepResult);
		}
	}
}
