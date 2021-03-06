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

	// Get auto possessed when spawned or placed in the world
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// Setup default name for parameter name for hit flash damage feedback
	DamagedMaterialParameterName = TEXT("TimeToHit");
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
	if (Delta < 0.0f)
	{
		// If character get killed
		if (NewValue <= 0.0f)
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
		else
		{
			// if instigator is not null and not other ai character / self, then set it as target actor
			if (InstigatorActor && !InstigatorActor->GetClass()->IsChildOf(StaticClass()))
			{
				SetTargetActor(InstigatorActor);
			}
		}

		// Damage feedback - hit flash
		GetMesh()->SetScalarParameterValueOnMaterials(DamagedMaterialParameterName, GetWorld()->GetTimeSeconds());
	}
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	SetTargetActor(Pawn);
	DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		UBlackboardComponent* BlackboardComp = AIC->GetBlackboardComponent();

		if (ensure(BlackboardComp))
		{
			BlackboardComp->SetValueAsObject("TargetActor", NewTarget);
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
