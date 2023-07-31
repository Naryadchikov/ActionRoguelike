// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"

#include "Components/SAttributeComponent.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount)
{
	if (USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(TargetActor))
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -DamageAmount);
	}

	return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float DamageAmount,
                                                       const FHitResult& HitResult)
{
	if (ApplyDamage(DamageCauser, TargetActor, DamageAmount))
	{
		UPrimitiveComponent* HitComp = HitResult.GetComponent();

		if (HitComp && HitComp->IsSimulatingPhysics(HitResult.BoneName))
		{
			// TODO: Fix direction of the impact to be not the normal of the surface but the direction of the flying projectile
			HitComp->AddImpulseAtLocation(-DamageAmount * 5000.0f * HitResult.ImpactNormal, HitResult.ImpactPoint,
			                              HitResult.BoneName);
		}

		return true;
	}

	return false;
}
