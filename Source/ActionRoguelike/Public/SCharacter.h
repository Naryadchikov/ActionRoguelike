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
class USInteractionComponent;
class USpringArmComponent;

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

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
