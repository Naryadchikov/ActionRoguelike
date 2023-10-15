// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "SAction_ProjectileAttack.generated.h"

class ASProjectileBase;
class UAnimMontage;
class UParticleSystem;


UCLASS()
class ACTIONROGUELIKE_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()

public:
	USAction_ProjectileAttack();

protected:
	// Projectile class
	UPROPERTY(EditAnywhere, Category = "Config|Attack")
	TSubclassOf<ASProjectileBase> ProjectileClass;

	// Socket name for spawning projectiles
	UPROPERTY(EditAnywhere, Category = "Config|Attack")
	FName ProjectileSpawnSocketName;

	// Primary attack animation montage
	UPROPERTY(EditAnywhere, Category = "Config|Attack")
	UAnimMontage* AttackAnim;

	// Delay after input received and spawning attack projectile
	UPROPERTY(EditAnywhere, Category = "Config|Attack")
	float AttackExecutionDelay;

	// Delay after input received and spawning attack projectile
	UPROPERTY(EditDefaultsOnly, Category = "Config|Attack")
	UParticleSystem* AttackCastEffect;

	FTimerHandle TimerHandle_ProjectileAttack;

	void SpawnProjectile(ACharacter* InstigatorCharacter);

public:
	virtual void StartAction_Implementation(AActor* Instigator) override;
};
