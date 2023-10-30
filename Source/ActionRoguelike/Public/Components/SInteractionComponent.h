// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"

class USWorldUserWidget;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROGUELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USInteractionComponent();

protected:
	// Interaction radius around the owner
	UPROPERTY(EditDefaultsOnly, Category = "Config|Interaction")
	float InteractionRadius;

	UPROPERTY() // GC will destroy this object when no longer referenced
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, Category = "Config|UI")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;

	UPROPERTY()
	USWorldUserWidget* DefaultWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "Config|Interaction")
	TEnumAsByte<ECollisionChannel> InteractionChannel;

	void FindBestInteractable();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// Interact action
	void PrimaryInteract();
};
