// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/PowerUps/SPowerUpBase.h"

#include "Components/SphereComponent.h"

// Sets default values
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
	// Add additional checks on InstigatorPawn in child classes and do not forget to call parent 'IsUsable'
	return !GetWorld()->GetTimerManager().TimerExists(TimerHandle_Interactable);
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
		Deactivate();
		GetWorldTimerManager().SetTimer(TimerHandle_Interactable, this, &ASPowerUpBase::Reactivate, Cooldown);
	}
}
