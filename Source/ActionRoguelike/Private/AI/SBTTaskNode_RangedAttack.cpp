// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTaskNode_RangedAttack.h"

#include "AIController.h"
#include "SProjectileBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SAttributeComponent.h"
#include "GameFramework/Character.h"

USBTTaskNode_RangedAttack::USBTTaskNode_RangedAttack()
{
	MaxBulletSpread = 3.0f;
}

EBTNodeResult::Type USBTTaskNode_RangedAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* OwnerController = OwnerComp.GetAIOwner();
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp && OwnerController))
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(OwnerController->GetPawn());
		const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor"));

		if (OwnerCharacter == nullptr || TargetActor == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		// If target actor is dead already
		if (!USAttributeComponent::IsActorAlive(TargetActor))
		{
			return EBTNodeResult::Failed;
		}

		// Perform check for owner mesh and muzzle socket 
		const USkeletalMeshComponent* OwnerCharacterMesh = OwnerCharacter->GetMesh();

		if (!IsValid(OwnerCharacterMesh) || !OwnerCharacterMesh->DoesSocketExist(ProjectileSpawnSocketName))
		{
			return EBTNodeResult::Failed;
		}

		// Calculate Muzzle Rotation and Location
		const FVector MuzzleLocation = OwnerCharacterMesh->GetSocketLocation(ProjectileSpawnSocketName);
		const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		FRotator MuzzleRotation = Direction.Rotation();

		// Create some weapon accuracy
		MuzzleRotation.Pitch += FMath::RandRange(0.0f, MaxBulletSpread);
		MuzzleRotation.Yaw += FMath::RandRange(-MaxBulletSpread, MaxBulletSpread);

		// Spawn attack projectile
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Params.Instigator = OwnerCharacter;

		const AActor* SpawnedProjectile = GetWorld()->SpawnActor<AActor>(
			ProjectileClass, MuzzleLocation, MuzzleRotation, Params);

		return SpawnedProjectile ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}
