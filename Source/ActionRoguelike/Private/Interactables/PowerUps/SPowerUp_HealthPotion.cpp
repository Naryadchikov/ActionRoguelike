// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/PowerUps/SPowerUp_HealthPotion.h"

#include "Components/SAttributeComponent.h"

ASPowerUp_HealthPotion::ASPowerUp_HealthPotion()
{
	HealValue = 30.0f;
	CreditCost = 50;
}

bool ASPowerUp_HealthPotion::IsUsable(APawn* InstigatorPawn) const
{
	bool bUsable = false;

	if (const USAttributeComponent* InstigatorAttrComp = Cast<USAttributeComponent>(
		InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass())))
	{
		bUsable = !InstigatorAttrComp->IsHealthMax();
	}

	return Super::IsUsable(InstigatorPawn) && bUsable;
}

void ASPowerUp_HealthPotion::ApplyPowerUp(APawn* InstigatorPawn)
{
	if (USAttributeComponent* InstigatorAttrComp = Cast<USAttributeComponent>(
		InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass())))
	{
		InstigatorAttrComp->ApplyHealthChange(InstigatorPawn, HealValue);
	}
}
