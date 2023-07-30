// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SAICharacter.generated.h"

class UPawnSensingComponent;
class USAttributeComponent;
class UUserWidget;
class USWorldUserWidget;

UCLASS()
class ACTIONROGUELIKE_API ASAICharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// Material parameter name for hit flash damage feedback
	UPROPERTY(EditDefaultsOnly, Category = "Config|Damaged")
	FName DamagedMaterialParameterName;

	// Health widget to spawn on damage received
	UPROPERTY(EditDefaultsOnly, Category = "Config|UI")
	TSubclassOf<UUserWidget> HealthBarWidgetClass;

	// Health widget initial world offset
	UPROPERTY(EditDefaultsOnly, Category = "Config|UI")
	FVector HealthBarWorldOffset;

	UPROPERTY(BlueprintReadOnly)
	USWorldUserWidget* HealthBarWidget;

public:
	ASAICharacter();

protected:
	/* Components */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	// Allow actors to initialize themselves on the C++ side after all of their components have been initialized
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue, float Delta);

	UFUNCTION()
	void OnPawnSeen(APawn* Pawn);

	UFUNCTION()
	void SetTargetActor(AActor* NewTarget);

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Attributes")
	bool IsAlive() const;

	virtual void SpawnDefaultController() override;
};
