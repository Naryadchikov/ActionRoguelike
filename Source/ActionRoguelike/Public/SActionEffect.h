// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actions/SAction.h"
#include "SActionEffect.generated.h"


UCLASS()
class ACTIONROGUELIKE_API USActionEffect : public USAction
{
	GENERATED_BODY()

public:
	USActionEffect();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Effect")
	float Duration;

	// Time between 'ticks' to apply the effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config|Effect")
	float Period;

	FTimerHandle DurationHandle;

	FTimerHandle PeriodHandle;

	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
	void ExecutePeriodEffect(AActor* Instigator);

public:
	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;
};
