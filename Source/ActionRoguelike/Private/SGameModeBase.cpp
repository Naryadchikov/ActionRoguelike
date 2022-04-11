// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "AI/SAICharacter.h"
#include "AI/SAIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed,
	                                SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	// Count alive bots on map
	if (DifficultyCurve)
	{
		int32 NumberOfAliveBots = 0;

		for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
		{
			const ASAICharacter* Bot = *It;

			if (Bot->IsAlive())
			{
				NumberOfAliveBots++;
			}
		}

		if (NumberOfAliveBots >= DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds))
		{
			UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn"));
			return;
		}
	}

	// Run find spawn location EQS query if number of alive bots is less than max count
	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
		this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
                                           EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query failed!"));
		return;
	}

	const TArray<FVector> LocationsToSpawn = QueryInstance->GetResultsAsLocations();

	if (LocationsToSpawn.Num() > 0)
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, LocationsToSpawn[0], FRotator::ZeroRotator);
	}
}

ASAIController* ASGameModeBase::FindFreeController(TSubclassOf<AController> AIControllerClass)
{
	if (FreeControllers.IsEmpty())
	{
		UE_LOG(LogTemp, Log, TEXT("No free controllers to asign."));
		return nullptr;
	}

	if (!AIControllerClass->IsChildOf(ASAIController::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("AIControllerClass is not child of 'ASAIController'."));
		return nullptr;
	}

	// Find free controller of class AIControllerClass
	ASAIController* ControllerToFind = nullptr;
	uint32 FoundIndex = FreeControllers.Num();

	while (FoundIndex > 0)
	{
		ASAIController* FreeController = FreeControllers[FoundIndex - 1];

		if (FreeController->IsA(AIControllerClass))
		{
			ControllerToFind = FreeController;
			break;
		}

		FoundIndex--;
	}

	// Remove found controller from array
	if (ControllerToFind)
	{
		FreeControllers.RemoveAt(FoundIndex - 1);
	}

	return ControllerToFind;
}

void ASGameModeBase::MarkControllerAsFree(ASAIController* ControllerToMark)
{
	FreeControllers.Add(ControllerToMark);
}
