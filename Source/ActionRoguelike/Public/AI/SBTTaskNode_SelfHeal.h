// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_Wait.h"
#include "SBTTaskNode_SelfHeal.generated.h"

UCLASS()
class ACTIONROGUELIKE_API USBTTaskNode_SelfHeal : public UBTTask_Wait
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedSkeletalMesh;

	EAnimationMode::Type PreviousAnimationMode;

	FTimerHandle TimerHandle_Heal;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual FString GetStaticDescription() const override;

protected:
	// Heal animation montage
	UPROPERTY(EditAnywhere, Category = "AI")
	UAnimMontage* HealAnim;

	void RestorePreviousAnimState() const;
};
