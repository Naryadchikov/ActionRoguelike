// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTaskNode_RangedAttack.generated.h"

class ASProjectileBase;

UCLASS()
class ACTIONROGUELIKE_API USBTTaskNode_RangedAttack : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// Socket name for spawning projectiles
	UPROPERTY(EditAnywhere, Category = "AI")
	FName ProjectileSpawnSocketName;

	// Attack projectile class
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ASProjectileBase> ProjectileClass;
};
