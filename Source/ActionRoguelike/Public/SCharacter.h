// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class USActionComponent;
class ASDashProjectile;
class ASMagicProjectile;
class ASProjectileBase;
class UAnimMontage;
class UCameraComponent;
class USAttributeComponent;
class USInteractionComponent;
class USpringArmComponent;
class UParticleSystem;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// Material parameter name for hit flash damage feedback
	UPROPERTY(EditDefaultsOnly, Category = "Config|Damaged")
	FName DamagedMaterialParameterName;

public:
	ASCharacter();

protected:
	/* Components */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Allow actors to initialize themselves on the C++ side after all of their components have been initialized
	virtual void PostInitializeComponents() override;

	// Move forward on player input
	void MoveForward(float Value);

	// Move right on player input
	void MoveRight(float Value);

	// Start sprinting
	void StartSprint();

	// Stop sprinting
	void StopSprint();

	// Player Primary Attack
	void PrimaryAttack();

	// Player Secondary Attack - Black Hole
	void BlackHoleAttack();

	// Dash projectile
	void Dash();

	// Player Primary Interact action
	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Debug heal self command
	UFUNCTION(Exec)
	void HealSelf(const float Amount = 100.0f);

	// Debug kill all bots command
	UFUNCTION(Exec)
	void KillAll();
};
