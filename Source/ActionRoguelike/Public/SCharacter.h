// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class ASMagicProjectile;
class UAnimMontage;
class UCameraComponent;
class USInteractionComponent;
class USpringArmComponent;

UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// Primary attack projectile class
	UPROPERTY(EditAnywhere, Category = "Config|PrimaryAttack")
	TSubclassOf<ASMagicProjectile> ProjectileClass;

	// Primary attack spawn socket name
	UPROPERTY(EditAnywhere, Category = "Config|PrimaryAttack")
	FName ProjectileSpawnSocketName;

	// Primary attack animation montage
	UPROPERTY(EditAnywhere, Category = "Config|PrimaryAttack")
	UAnimMontage* AttackAnim;

	// Delay after input received and spawning attack projectile
	UPROPERTY(EditAnywhere, Category = "Config|PrimaryAttack")
	float AttackExecutionDelay;

	FTimerHandle TimerHandle_PrimaryAttack;

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

	// Spawn actual projectile in appropriate moment during primary attack animation
	void SpawnPrimaryAttackProjectile();

	// Player Primary Interact action
	void PrimaryInteract();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
