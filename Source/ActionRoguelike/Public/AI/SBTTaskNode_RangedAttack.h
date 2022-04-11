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

public:
	USBTTaskNode_RangedAttack();

protected:
	// Socket name for spawning projectiles
	UPROPERTY(EditAnywhere, Category = "AI")
	FName ProjectileSpawnSocketName;

	// Attack projectile class
	UPROPERTY(EditAnywhere, Category = "AI")
	TSubclassOf<ASProjectileBase> ProjectileClass;

	// Positive max bullet spread
	// result spread will be in [-MaxBulletSpread; MaxBulletSpread] range for Yaw and [0; MaxBulletSpread] for Pitch
	UPROPERTY(EditAnywhere, Category = "AI", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxBulletSpread;
};
