// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SWorldUserWidget.generated.h"

class USizeBox;

UCLASS()
class ACTIONROGUELIKE_API USWorldUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	// Parent size box to set size of this widget
	UPROPERTY(meta = (BindWidget))
	USizeBox* ParentSizeBox;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	// Actor to attach this widget to
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	AActor* AttachedActor;

	// Widget world offset
	UPROPERTY(EditAnywhere, Category = "UI")
	FVector WorldOffset;
};
