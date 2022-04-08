// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("Behavior Tree is nullptr! Please assign BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}

	// Temporary solution: set player location to BB
	// if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
	// {
	// 	GetBlackboardComponent()->SetValueAsVector("MoveToLocation", PlayerPawn->GetActorLocation());
	// 	GetBlackboardComponent()->SetValueAsObject("TargetActor", PlayerPawn);
	// }
}
