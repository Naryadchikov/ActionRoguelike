// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "SPowerUpBase.generated.h"

class USphereComponent;

UCLASS()
class ACTIONROGUELIKE_API ASPowerUpBase : public AActor, public ISInteractableInterface
{
	GENERATED_BODY()

protected:
	// Cooldown time after power-up use
	UPROPERTY(EditAnywhere, Category = "Potion")
	float Cooldown;

	FTimerHandle TimerHandle_Interactable;

public:
	ASPowerUpBase();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* BaseMesh;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintPure)
	virtual bool IsUsable(APawn* InstigatorPawn) const;

	virtual void ApplyPowerUp(APawn* InstigatorPawn);

	virtual void Deactivate();

	virtual void Reactivate();

public:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
