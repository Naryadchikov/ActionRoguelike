// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTaskNode_RangedAttack.h"

#include "AIController.h"
#include "SProjectileBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

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

		// Perform check for owner mesh and muzzle socket 
		const USkeletalMeshComponent* OwnerCharacterMesh = OwnerCharacter->GetMesh();

		if (!IsValid(OwnerCharacterMesh) || !OwnerCharacterMesh->DoesSocketExist(ProjectileSpawnSocketName))
		{
			return EBTNodeResult::Failed;
		}

		// Calculate Muzzle Rotation and Location
		const FVector MuzzleLocation = OwnerCharacterMesh->GetSocketLocation(ProjectileSpawnSocketName);
		const FVector Direction = TargetActor->GetActorLocation() - MuzzleLocation;
		const FRotator MuzzleRotation = Direction.Rotation();

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
