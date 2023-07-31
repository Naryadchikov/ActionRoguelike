// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SCharacter.h"
#include "AI/SAICharacter.h"
#include "AI/SAIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"

// Console variable for bot spawning
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("sar.SpawnBots"), true, TEXT("Enable bot spawning via timer."),
                                                ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	PlayerRespawnDelay = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed,
	                                SpawnTimerInterval, true);
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	// Turn off bot spawning if CVarSpawnBots is false 
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning is disabled via cvar `CVarSpawnBots`."));

		return;
	}

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

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		// Removing player character from controller to get new fresh copy from calling restart
		Controller->UnPossess();

		RestartPlayer(Controller);
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

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* KillerActor)
{
	if (const ASCharacter* Player = Cast<ASCharacter>(VictimActor))
	{
		FTimerHandle TimerHandle_RespawnDelay; // should be local variable for multiplayer
		FTimerDelegate TimerDelegate_RespawnDelay;

		TimerDelegate_RespawnDelay.BindUFunction(this, FName("RespawnPlayerElapsed"), Player->GetController());
		Player->GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, TimerDelegate_RespawnDelay,
		                                        PlayerRespawnDelay, false);
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor),
	       *GetNameSafe(KillerActor));
}
