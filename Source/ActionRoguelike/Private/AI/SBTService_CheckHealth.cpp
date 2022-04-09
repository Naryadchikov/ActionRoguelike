// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SAttributeComponent.h"

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const AAIController* AIC = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(AIC && BlackboardComp))
	{
		const APawn* ControlledPawn = AIC->GetPawn();

		if (ensure(ControlledPawn))
		{
			const USAttributeComponent* AttrComp = Cast<USAttributeComponent>(
				ControlledPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

			if (ensureMsgf(
				AttrComp, TEXT("Controlled Pawn must have AttributeComponent!")))
			{
				BlackboardComp->SetValueAsBool(LowHPKey.SelectedKeyName, AttrComp->GetCurrentHealth() <= LowHPValue);
			}
		}
	}
}
