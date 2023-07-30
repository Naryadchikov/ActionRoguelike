// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTaskNode_SelfHeal.h"

#include "AIController.h"
#include "Components/SAttributeComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTaskNode_SelfHeal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIC = OwnerComp.GetAIOwner();

	if (ensure(AIC))
	{
		APawn* ControlledPawn = AIC->GetPawn();

		if (ensure(ControlledPawn))
		{
			USAttributeComponent* AttrComp = Cast<USAttributeComponent>(
				ControlledPawn->GetComponentByClass(USAttributeComponent::StaticClass()));

			if (ensureMsgf(
				AttrComp, TEXT("Controlled Pawn must have AttributeComponent!")))
			{
				const ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);

				if (HealAnim && ControlledCharacter && ControlledCharacter->GetMesh())
				{
					// If character is healing already, do not do it again
					if (ControlledCharacter->GetWorldTimerManager().TimerExists(TimerHandle_Heal))
					{
						return EBTNodeResult::Failed;
					}

					// Play heal anim montage
					USkeletalMeshComponent* SkeletalMesh = ControlledCharacter->GetMesh();
					PreviousAnimationMode = SkeletalMesh->GetAnimationMode();
					CachedSkeletalMesh = SkeletalMesh;

					SkeletalMesh->PlayAnimation(HealAnim, false);

					const float HealAnimDuration = HealAnim->GetPlayLength();

					// Create timer delegate to call apply health change after anim montage finishes
					const FTimerDelegate TimerDelegate_Heal = FTimerDelegate::CreateLambda(
						[this, ControlledPawn](USAttributeComponent* AttributeComponent)
						{
							AttributeComponent->ApplyHealthChange(ControlledPawn, AttributeComponent->GetMaxHealth());
							this->RestorePreviousAnimState();
						}, AttrComp
					);

					ControlledCharacter->GetWorldTimerManager().SetTimer(
						TimerHandle_Heal, TimerDelegate_Heal, HealAnimDuration, false);

					// Set wait time for task
					SetNextTickTime(NodeMemory, HealAnimDuration);

					return EBTNodeResult::InProgress;
				}

				// If heal anim montage is not specified heal instantly
				AttrComp->ApplyHealthChange(ControlledPawn, AttrComp->GetMaxHealth());
				UE_LOG(LogTemp, Warning, TEXT("Heal animation is not set in %s BTTask"), *GetNameSafe(this));

				return EBTNodeResult::Succeeded;
			}

			return EBTNodeResult::Failed;
		}

		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Failed;
}

EBTNodeResult::Type USBTTaskNode_SelfHeal::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIC = OwnerComp.GetAIOwner();

	if (HealAnim && AIC && TimerHandle_Heal.IsValid())
	{
		AIC->GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Heal);
	}

	TimerHandle_Heal.Invalidate();

	RestorePreviousAnimState();

	return EBTNodeResult::Aborted;
}

FString USBTTaskNode_SelfHeal::GetStaticDescription() const
{
	if (!HealAnim)
	{
		return FString::Printf(TEXT("Instant heal, no anim montage specified!"));
	}

	return FString::Printf(TEXT("Heal time: %.1fs"), HealAnim->GetPlayLength());
}

void USBTTaskNode_SelfHeal::RestorePreviousAnimState() const
{
	// Set anim mode back to animation blueprint
	if (CachedSkeletalMesh && PreviousAnimationMode == EAnimationMode::AnimationBlueprint)
	{
		CachedSkeletalMesh->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
}
