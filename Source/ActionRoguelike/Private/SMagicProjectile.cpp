// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "Components/SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
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
	if (OtherActor)
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(
			OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));

		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-20.0f);

			Destroy();
		}
	}
}
