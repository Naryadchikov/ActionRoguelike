// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "SInteractableInterface.h"
#include "ProfilingDebugging/CookStats.h"

// Console variable for showing debug info
static TAutoConsoleVariable<bool> CVarShowInteractionDebugInfo(
	TEXT("sar.ShowInteractionDebugInfo"), false, TEXT("Show debug information for interactions"), ECVF_Cheat);

USInteractionComponent::USInteractionComponent()
{
	InteractionRadius = 250.0f;
}

void USInteractionComponent::PrimaryInteract()
{
	bool bShowInteractionDebugInfo = CVarShowInteractionDebugInfo.GetValueOnGameThread();

	AActor* MyOwner = GetOwner();

	// Get owner eye location and rotation
	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	const FVector EyeDir = EyeRotation.Vector();

	// Sweep for all dynamic objects around the owner
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	TArray<FHitResult> Hits;

	FCollisionShape Shape;
	Shape.SetSphere(InteractionRadius);

	const bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, EyeLocation, FQuat::Identity,
	                                                             ObjectQueryParams, Shape);

	// Draw debug interaction sphere
	if (bShowInteractionDebugInfo)
	{
		DrawDebugSphere(GetWorld(), MyOwner->GetActorLocation(), InteractionRadius, 32, FColor::Yellow, false, 2.0f);
	}

	// if we hit something
	if (bBlockingHit)
	{
		// filter for interactable and visible actors
		// for visibility check: comparing world time with 'LastRenderTimeOnScreen' of primitive component that was hit
		// NOTE: 'WasRecentlyRendered()' and 'GetLastRenderTime()' on actor object is not giving correct result
		TArray<FHitResult> FilteredHits = Hits.FilterByPredicate([this](const FHitResult Hit)
		{
			const AActor* HitActor = Hit.GetActor();
			const UPrimitiveComponent* PrimitiveHitComponent = Cast<UPrimitiveComponent>(Hit.GetComponent());

			return HitActor
				&& PrimitiveHitComponent
				&& HitActor->Implements<USInteractableInterface>()
				&& GetWorld()->GetTimeSeconds() - PrimitiveHitComponent->GetLastRenderTimeOnScreen() < 0.1f;
		});

		// if there are any
		if (FilteredHits.Num() > 0)
		{
			// sort by closest to the actor sight
			FilteredHits.Sort([EyeLocation, EyeDir](const FHitResult HitA, const FHitResult HitB)
			{
				float DotA = (HitA.ImpactPoint - EyeLocation).GetSafeNormal() | EyeDir;
				float DotB = (HitB.ImpactPoint - EyeLocation).GetSafeNormal() | EyeDir;
				return DotA > DotB;
			});

			// Interact with closest to the sight actor
			ISInteractableInterface::Execute_Interact(FilteredHits[0].GetActor(), Cast<APawn>(MyOwner));

			// Draw debug spheres on all swept interactable objects' impact points
			if (bShowInteractionDebugInfo)
			{
				FColor DebugLineColor = FColor::Green;
				for (FHitResult Hit : FilteredHits)
				{
					DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 30.f, 32, DebugLineColor, false, 2.0f);
					DebugLineColor = FColor::Red;
				}
			}
		}
	}
}
