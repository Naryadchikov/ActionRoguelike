// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"

USAttributeComponent::USAttributeComponent()
{
	// Set default health value
	Health = 100.0f;
	MaxHealth = 100.0f;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (Health <= 0.0f ||
		Health >= MaxHealth && Delta > 0.0f)
	{
		return false;
	}

	const float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(InstigatorActor, this, Health, Health - OldHealth);

	return true;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsHealthMax() const
{
	return FMath::IsNearlyEqual(Health, MaxHealth);
}

float USAttributeComponent::GetCurrentHealth() const
{
	return Health;
}

float USAttributeComponent::GetMaxHealth() const
{
	return MaxHealth;
}

/* Static functions */
USAttributeComponent* USAttributeComponent::GetAttributes(const AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(const AActor* FromActor)
{
	if (const USAttributeComponent* AttributeComponent = GetAttributes(FromActor))
	{
		return AttributeComponent->IsAlive();
	}

	return false;
}
