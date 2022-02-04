// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class ASMagicProjectile;
class UCameraComponent;
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

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Move forward on player input
	void MoveForward(float Value);

	// Move right on player input
	void MoveRight(float Value);

	// Player Primary Attack
	void PrimaryAttack();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
