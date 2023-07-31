// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"

#include "AIController.h"
#include "SGameModeBase.h"
#include "AI/SAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SAttributeComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "UI/SWorldUserWidget.h"

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

	// TODO: Remove this line once new collision channel for projectile is created
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);

	// Generate overlap events with the mesh to apply directional damage on projectile impact
	GetMesh()->SetGenerateOverlapEvents(true);
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

			// Destroy health bar widget
			if (HealthBarWidget)
			{
				HealthBarWidget->RemoveFromParent();
			}

			// Ragdoll
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			// Stop movement
			GetMovementComponent()->StopMovementImmediately();

			// Set life span
			SetLifeSpan(30.0f);
		}
		else
		{
			// if instigator is not null and not other ai character / self, then set it as target actor
			if (InstigatorActor && !InstigatorActor->GetClass()->IsChildOf(StaticClass()))
			{
				SetTargetActor(InstigatorActor);
			}

			// Spawn health bar widget on first damage received
			if (HealthBarWidget == nullptr)
			{
				HealthBarWidget = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);

				if (HealthBarWidget)
				{
					HealthBarWidget->AttachedActor = this;
					HealthBarWidget->WorldOffset = HealthBarWorldOffset;
					HealthBarWidget->AddToViewport();
				}
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
