// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/PowerUps/SPowerUpBase.h"
#include "SPowerUp_HealthPotion.generated.h"

UCLASS()
class ACTIONROGUELIKE_API ASPowerUp_HealthPotion : public ASPowerUpBase
{
	GENERATED_BODY()

public:
	ASPowerUp_HealthPotion();

protected:
	// Health restore value
	UPROPERTY(EditAnywhere, Category = "Potion")
	float HealValue;

	virtual bool IsUsable(APawn* InstigatorPawn) const override;

	virtual void ApplyPowerUp(APawn* InstigatorPawn) override;
};
