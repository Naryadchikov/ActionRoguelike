// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Interaction radius around the owner
	UPROPERTY(EditAnywhere, Category = "Config")
	float InteractionRadius;

public:
	USInteractionComponent();

	// Interact action
	void PrimaryInteract();
};
