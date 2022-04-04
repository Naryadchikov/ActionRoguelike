// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	// Set default health value
	Health = 100.0f;
	MaxHealth = 100.0f;
}


bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	if (Health <= 0.0f ||
		Health >= MaxHealth && Delta > 0.0f)
	{
		return false;
	}

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	return true;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}
