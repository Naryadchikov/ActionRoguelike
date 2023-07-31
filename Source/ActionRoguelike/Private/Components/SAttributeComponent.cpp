// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SAttributeComponent.h"

#include "SGameModeBase.h"

// Console variable for global damage multiplier
static TAutoConsoleVariable<float> CVarDamageMultiplier(
	TEXT("sar.DamageMultiplier"), 1.0f, TEXT("Global damage modifier for Attribute component."), ECVF_Cheat);

USAttributeComponent::USAttributeComponent()
{
	// Set default health value
	Health = 100.0f;
	MaxHealth = 100.0f;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	if (Health <= 0.0f
		|| Health >= MaxHealth && Delta > 0.0f
		|| Delta < 0.0f && !GetOwner()->CanBeDamaged()) // for god mode ("god" console command)
	{
		return false;
	}

	// Add damage multiplier from CVarDamageMultiplier
	if (Delta < 0.0f)
	{
		const float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();

		Delta *= DamageMultiplier;
	}

	// Calculate new health value
	const float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, MaxHealth);

	const float ActualDelta = Health - OldHealth;

	OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	// Died
	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();

		if (GM)
		{
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

	return true;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -MaxHealth);
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
