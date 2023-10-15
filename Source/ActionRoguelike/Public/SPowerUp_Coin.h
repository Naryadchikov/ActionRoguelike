// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/PowerUps/SPowerUpBase.h"
#include "SPowerUp_Coin.generated.h"


UCLASS()
class ACTIONROGUELIKE_API ASPowerUp_Coin : public ASPowerUpBase
{
	GENERATED_BODY()

protected:
	// Credits to be rewarded
	UPROPERTY(EditAnywhere, Category = "Reward")
	int32 RewardCredits;

	virtual void ApplyPowerUp(APawn* InstigatorPawn) override;
};
