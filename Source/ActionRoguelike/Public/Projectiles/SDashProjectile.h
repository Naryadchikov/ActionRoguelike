// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASDashProjectile : public ASProjectileBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASDashProjectile();

protected:
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ExplosionEffectComp;

	// Time after spawning before freezing teleportation location and explode
	UPROPERTY(EditAnywhere, Category = "Config")
	float ExplosionDelay;

	// Teleportation delay after explosion
	UPROPERTY(EditAnywhere, Category = "Config")
	float TeleportationDelay;
	
	FTimerHandle TimerHandle_Explosion;

	FTimerHandle TimerHandle_Teleportation;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void OnDashProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                         FVector NormalImpulse, const FHitResult& Hit);

	void Explode(bool bHit, FVector HitLocation);

	void TeleportInstigator(FVector TeleportLocation);
};
