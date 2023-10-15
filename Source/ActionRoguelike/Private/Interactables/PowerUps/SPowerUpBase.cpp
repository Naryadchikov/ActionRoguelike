// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/PowerUps/SPowerUpBase.h"

#include "SPlayerState.h"
#include "Components/SphereComponent.h"

ASPowerUpBase::ASPowerUpBase()
{
	// Set up collision component
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("PowerUp");
	RootComponent = SphereComp;

	// Set up projectile effect component
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	BaseMesh->SetupAttachment(RootComponent);

	// Setup default cooldown time
	Cooldown = 10.0f;

	// Default credit cost
	CreditCost = 0;
}

void ASPowerUpBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clear All timers that belong to this actor.
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ASPowerUpBase::Deactivate()
{
	BaseMesh->SetVisibility(false);
}

void ASPowerUpBase::Reactivate()
{
	BaseMesh->SetVisibility(true);
}

bool ASPowerUpBase::IsUsable(APawn* InstigatorPawn) const
{
	if (!InstigatorPawn)
	{
		return false;
	}

	bool bPurchasable = true;

	if (const ASPlayerState* PlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState()))
	{
		bPurchasable = PlayerState->GetCredits() >= CreditCost;
	}

	// Add additional checks on InstigatorPawn in child classes and do not forget to call parent 'IsUsable'
	return bPurchasable && !GetWorld()->GetTimerManager().TimerExists(TimerHandle_Interactable);
}

void ASPowerUpBase::ApplyPowerUp(APawn* InstigatorPawn)
{
	// Do something in child classes
}

void ASPowerUpBase::Interact_Implementation(APawn* InstigatorPawn)
{
	if (IsUsable(InstigatorPawn))
	{
		ApplyPowerUp(InstigatorPawn);

		if (ASPlayerState* PlayerState = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState()))
		{
			PlayerState->RemoveCredits(CreditCost);
		}

		Deactivate();

		GetWorldTimerManager().SetTimer(TimerHandle_Interactable, this, &ASPowerUpBase::Reactivate, Cooldown);
	}
}
