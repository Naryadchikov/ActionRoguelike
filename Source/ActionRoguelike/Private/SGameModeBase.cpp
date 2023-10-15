// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "EngineUtils.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "AI/SAIController.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "Interactables/PowerUps/SPowerUpBase.h"

// Console variable for bot spawning
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("sar.SpawnBots"), true, TEXT("Enable bot spawning via timer."),
                                                ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	PlayerRespawnDelay = 2.0f;

	CreditsPerKill = 20;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed,
	                                SpawnTimerInterval, true);

	// Make sure we have assigned at least one power-up class
	if (ensure(PowerupClasses.Num() > 0))
	{
		// Run EQS to find potential power-up spawn locations
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(
			this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);

		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}
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
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
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

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance,
                                                  EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// Keep used locations to easily check distance between points
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;

	// Break out if we reached the desired count or if we have no more potential positions remaining
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// Pick a random location from remaining points.
		const int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);
		FVector PickedLocation = Locations[RandomLocationIndex];

		// Remove to avoid picking again
		Locations.RemoveAt(RandomLocationIndex);

		// Check minimum distance requirement
		bool bValidLocation = true;

		for (FVector OtherLocation : UsedLocations)
		{
			const float DistanceTo = (PickedLocation - OtherLocation).Size();

			if (DistanceTo < RequiredPowerupDistance)
			{
				// Show skipped locations due to distance
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				// too close, skip to next attempt
				bValidLocation = false;
				break;
			}
		}

		// Failed the distance test
		if (!bValidLocation)
		{
			continue;
		}

		// Pick a random powerup-class
		const int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<ASPowerUpBase> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<ASPowerUpBase>(RandomPowerupClass, PickedLocation + FVector(0.0f, 0.0f, 100.0f),
		                                      FRotator::ZeroRotator);

		// Keep for distance checks
		UsedLocations.Add(PickedLocation);

		SpawnCounter++;
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

	// Give credits for kill
	if (const APawn* KillerPawn = Cast<APawn>(KillerActor))
	{
		if (ASPlayerState* PlayerState = Cast<ASPlayerState>(KillerPawn->GetPlayerState()))
		{
			PlayerState->AddCredits(CreditsPerKill);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor),
	       *GetNameSafe(KillerActor));
}
