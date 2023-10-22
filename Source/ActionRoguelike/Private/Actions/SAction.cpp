// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction.h"
#include "Components/SActionComponent.h"

void USAction::StartAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Running: %s"), *GetNameSafe(this));

	USActionComponent* ActionComp = GetOwningComponent();

	ActionComp->ActiveGameplayTags.AppendTags(GrantsTags);

	bIsRunning = true;
}

void USAction::StopAction_Implementation(AActor* Instigator)
{
	UE_LOG(LogTemp, Log, TEXT("Stopped: %s"), *GetNameSafe(this));

	ensureAlways(bIsRunning);

	USActionComponent* ActionComp = GetOwningComponent();

	/* If several actions with same tags are performed simultaneously
	 * and have different duration we will remove the tag earlier than we should.
	 * Solution is to use a counter for each tag and remove the tag only when counter is 0.
	 * There is example in Lyra project that is called FGameplayTagStackContainer
	 */
	ActionComp->ActiveGameplayTags.RemoveTags(GrantsTags);

	bIsRunning = false;
}

UWorld* USAction::GetWorld() const
{
	// Outer is set when creating action via NewObject<T> in ActionComponent
	if (const UActorComponent* Comp = Cast<UActorComponent>(GetOuter()))
	{
		return Comp->GetWorld();
	}

	return nullptr;
}

USActionComponent* USAction::GetOwningComponent() const
{
	return Cast<USActionComponent>(GetOuter());
}

bool USAction::CanStart_Implementation(AActor* Instigator)
{
	if (IsRunning())
	{
		return false;
	}

	const USActionComponent* ActionComp = GetOwningComponent();

	if (ActionComp->ActiveGameplayTags.HasAny(BlockedTags))
	{
		return false;
	}

	return true;
}
