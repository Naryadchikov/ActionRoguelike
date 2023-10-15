// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp_Coin.h"

#include "SPlayerState.h"

void ASPowerUp_Coin::ApplyPowerUp(APawn* InstigatorPawn)
{
	if (ASPlayerState* PlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState()))
	{
		PlayerState->AddCredits(RewardCredits);
	}
}
