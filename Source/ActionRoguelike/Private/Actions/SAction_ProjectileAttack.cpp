// Fill out your copyright notice in the Description page of Project Settings.


#include "Actions/SAction_ProjectileAttack.h"

#include "SProjectileBase.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	ProjectileSpawnSocketName = "Muzzle_01";
	AttackExecutionDelay = 0.2f;
}

void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	ACharacter* Character = Cast<ACharacter>(Instigator);

	if (!Character)
	{
		return;
	}

	if (AttackAnim)
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_ProjectileAttack))
		{
			Character->PlayAnimMontage(AttackAnim);

			if (AttackCastEffect)
			{
				UGameplayStatics::SpawnEmitterAttached(AttackCastEffect, Character->GetMesh(),
				                                       ProjectileSpawnSocketName,
				                                       FVector::ZeroVector, FRotator::ZeroRotator,
				                                       EAttachLocation::SnapToTarget);
			}

			FTimerDelegate TimerDelegate_ProjectileAttack;
			TimerDelegate_ProjectileAttack.BindUObject(this, &USAction_ProjectileAttack::SpawnProjectile, Character);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_ProjectileAttack, TimerDelegate_ProjectileAttack,
			                                       AttackExecutionDelay, false);
		}
	}
	else
	{
		SpawnProjectile(Character);
	}
}

void USAction_ProjectileAttack::SpawnProjectile(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		// find attack spawn location, which is the hand socket
		const FVector SpawnLocation = [this, InstigatorCharacter]()
		{
			if (IsValid(InstigatorCharacter->GetMesh()) && InstigatorCharacter->GetMesh()->DoesSocketExist(
				ProjectileSpawnSocketName))
			{
				return InstigatorCharacter->GetMesh()->GetSocketLocation(ProjectileSpawnSocketName);
			}
			UE_LOG(LogTemp, Warning, TEXT("Mesh is invalid or socket with 'ProjectileSpawnSocketName' does not exist"));
			return InstigatorCharacter->GetActorLocation();
		}();

		// find spawn rotation by line-tracing from camera to the world, finding desired 'impact' location
		const FVector Start = InstigatorCharacter->GetPawnViewLocation();
		const FVector End = Start + InstigatorCharacter->GetControlRotation().Vector() * 5000.f;

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(InstigatorCharacter);

		FCollisionShape Shape;
		Shape.SetSphere(20.f);

		FHitResult Hit;
		const bool bBlockingHit = GetWorld()->SweepSingleByObjectType(Hit, Start, End, FQuat::Identity,
		                                                              ObjectQueryParams, Shape, CollisionQueryParams);

		const FRotator SpawnRotation = FRotationMatrix::MakeFromX(
				(bBlockingHit ? Hit.ImpactPoint : End) - SpawnLocation).
			Rotator();

		// spawn projectile
		const FTransform SpawnTM = FTransform(SpawnRotation, SpawnLocation);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;

		GetWorld()->SpawnActor<ASProjectileBase>(ProjectileClass, SpawnTM, SpawnParams);
	}

	StopAction(InstigatorCharacter);
}
