// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionEffect.h"

#include "Components/SActionComponent.h"

USActionEffect::USActionEffect()
{
	bAutoStart = true;
	Duration = 3.0f;
	Period = 1.0f;
}

void USActionEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	if (Duration > 0.0f)
	{
		FTimerDelegate DurationDelegate;

		DurationDelegate.BindUFunction(this, FName("StopAction"), Instigator);

		GetWorld()->GetTimerManager().SetTimer(DurationHandle, DurationDelegate, Duration, false);
	}

	if (Period > 0.0f)
	{
		FTimerDelegate PeriodDelegate;

		PeriodDelegate.BindUFunction(this, FName("ExecutePeriodEffect"), Instigator);

		GetWorld()->GetTimerManager().SetTimer(PeriodHandle, PeriodDelegate, Period, true);
	}
}

void USActionEffect::StopAction_Implementation(AActor* Instigator)
{
	if (GetWorld()->GetTimerManager().GetTimerRemaining(PeriodHandle) < KINDA_SMALL_NUMBER)
	{
		ExecutePeriodEffect(Instigator);
	}

	Super::StopAction_Implementation(Instigator);

	GetWorld()->GetTimerManager().ClearTimer(PeriodHandle);
	GetWorld()->GetTimerManager().ClearTimer(DurationHandle);

	if (USActionComponent* ActionComp = GetOwningComponent())
	{
		ActionComp->RemoveAction(this);
	}
}

void USActionEffect::ExecutePeriodEffect_Implementation(AActor* Instigator)
{
}
