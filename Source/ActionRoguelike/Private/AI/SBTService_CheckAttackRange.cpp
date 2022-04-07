// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckAttackRange.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckAttackRange::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// Check distance between AI pawn and target actor
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp))
	{
		if (const AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("TargetActor")))
		{
			const AAIController* OwnerController = OwnerComp.GetAIOwner();

			if (ensure(OwnerController))
			{
				const APawn* OwnerPawn = OwnerController->GetPawn();

				if (ensure(OwnerPawn))
				{
					const float DistanceTo =
						FVector::Distance(TargetActor->GetActorLocation(), OwnerPawn->GetActorLocation());
					const bool bWithinRange = DistanceTo < 2000.0f;

					const bool bInLineOfSight = bWithinRange ? OwnerController->LineOfSightTo(TargetActor) : false;

					BlackboardComp->SetValueAsBool(AttackRangeKey.SelectedKeyName, bInLineOfSight);
				}
			}
		}
	}
}
