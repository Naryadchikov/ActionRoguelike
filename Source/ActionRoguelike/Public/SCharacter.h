// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

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
	// Socket name for spawning projectiles
	UPROPERTY(EditAnywhere, Category = "Config|Attack")
	FName ProjectileSpawnSocketName;

	// Primary attack projectile class
	UPROPERTY(EditAnywhere, Category = "Config|PrimaryAttack")
	TSubclassOf<ASMagicProjectile> PrimaryProjectileClass;

	// Primary attack animation montage
	UPROPERTY(EditAnywhere, Category = "Config|PrimaryAttack")
	UAnimMontage* PrimaryAttackAnim;

	// Delay after input received and spawning primary attack projectile
	UPROPERTY(EditAnywhere, Category = "Config|PrimaryAttack")
	float PrimaryAttackExecutionDelay;

	// Delay after input received and spawning primary attack projectile
	UPROPERTY(EditDefaultsOnly, Category = "Config|PrimaryAttack")
	UParticleSystem* PrimaryAttackCastEffect;

	FTimerHandle TimerHandle_PrimaryAttack;

	// Secondary attack projectile class
	UPROPERTY(EditAnywhere, Category = "Config|SecondaryAttack")
	TSubclassOf<ASProjectileBase> SecondaryProjectileClass;

	// Secondary attack animation montage
	UPROPERTY(EditAnywhere, Category = "Config|SecondaryAttack")
	UAnimMontage* SecondaryAttackAnim;

	// Delay after input received and spawning secondary attack projectile
	UPROPERTY(EditAnywhere, Category = "Config|SecondaryAttack")
	float SecondaryAttackExecutionDelay;

	FTimerHandle TimerHandle_SecondaryAttack;

	// Dash projectile class
	UPROPERTY(EditAnywhere, Category = "Config|Dash")
	TSubclassOf<ASDashProjectile> DashProjectileClass;

	// Dash animation montage
	UPROPERTY(EditAnywhere, Category = "Config|Dash")
	UAnimMontage* DashAnim;

	// Delay after input received and spawning dash projectile
	UPROPERTY(EditAnywhere, Category = "Config|Dash")
	float DashExecutionDelay;

	FTimerHandle TimerHandle_Dash;

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

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Allow actors to initialize themselves on the C++ side after all of their components have been initialized
	virtual void PostInitializeComponents() override;

	// Move forward on player input
	void MoveForward(float Value);

	// Move right on player input
	void MoveRight(float Value);

	// Player Primary Attack
	void PrimaryAttack();

	// Player Secondary Attack - Black Hole
	void BlackHoleAttack();

	// Dash projectile
	void Dash();

	void SpawnProjectile(TSubclassOf<ASProjectileBase> ClassToSpawn);

	// Spawn primary attack projectile in appropriate moment during primary attack animation
	void SpawnPrimaryAttackProjectile();

	// Spawn black hole in appropriate moment during secondary attack animation
	void SpawnBlackHoleProjectile();

	// Spawn dash projectile in appropriate moment during dash animation
	void SpawnDashProjectile();

	// Player Primary Interact action
	void PrimaryInteract();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
