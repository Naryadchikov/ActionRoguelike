// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SProjectileBase.h"
#include "SMagicProjectile.generated.h"

class UParticleSystemComponent;
class UProjectileMovementComponent;
class USphereComponent;
class USoundBase;
class USoundAttenuation;
class UParticleSystem;
class UCameraShakeBase;

UCLASS()
class ACTIONROGUELIKE_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Config|Damage", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ProjectileDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Config|GameplayTags")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Sound")
	USoundBase* ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Sound")
	USoundAttenuation* ImpactSoundAttenuation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|FX")
	UParticleSystem* ImpactEmitterTemplate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|FX")
	TSubclassOf<UCameraShakeBase> WorldCameraShakeClass;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Allow actors to initialize themselves on the C++ side after all of their components have been initialized
	virtual void PostInitializeComponents() override;

	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                             FVector NormalImpulse, const FHitResult& Hit) override;
};
