// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "EngineUtils.h"
#include "SGameModeBase.h"
#include "AI/NavigationSystemBase.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SAttributeComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Perception/PawnSensingComponent.h"

ASAICharacter::ASAICharacter()
{
	// Set up Pawn Sensing Component
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");

	// Set up attribute component
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue,
                                    float Delta)
{
	// If character get killed
	if (NewValue <= 0.0f && Delta < 0.0f)
	{
		if (AAIController* AIC = Cast<AAIController>(GetController()))
		{
			//AIC->GetBrainComponent()->StopLogic("Killed");
			AIC->UnPossess();

			// If Controller has 'ASAIController' class mark it as free controller 
			if (ASAIController* S_AIC = Cast<ASAIController>(AIC))
			{
				ASGameModeBase* GM = Cast<ASGameModeBase>(GetWorld()->GetAuthGameMode());

				if (GM)
				{
					GM->MarkControllerAsFree(S_AIC);
				}
			}
		}

		// Stop movement and set life span
		GetMovementComponent()->StopMovementImmediately();
		SetLifeSpan(30.0f);
	}
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	AAIController* AIC = Cast<AAIController>(GetController());

	if (AIC)
	{
		UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();

		if (ensure(BlackboardComp))
		{
			BlackboardComp->SetValueAsObject("TargetActor", Pawn);

			DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
		}
	}
}

bool ASAICharacter::IsAlive() const
{
	return AttributeComp->IsAlive();
}

void ASAICharacter::SpawnDefaultController()
{
	if (GetController())
	{
		return;
	}

	if (AIControllerClass != nullptr)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.Instigator = GetInstigator();
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.OverrideLevel = GetLevel();
		SpawnInfo.ObjectFlags |= RF_Transient; // We never want to save AI controllers into a map

		AController* NewController = nullptr;

		// Try to find free already spawned controllers
		ASGameModeBase* GM = Cast<ASGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			NewController = GM->FindFreeController(AIControllerClass);
		}

		// Create new controller if we can't find free one
		if (!NewController)
		{
			NewController = GetWorld()->SpawnActor<AController>(AIControllerClass, GetActorLocation(),
			                                                    GetActorRotation(), SpawnInfo);
		}

		if (NewController)
		{
			// if successful will result in setting this->Controller 
			// as part of possession mechanics
			NewController->Possess(this);
		}
	}
}
