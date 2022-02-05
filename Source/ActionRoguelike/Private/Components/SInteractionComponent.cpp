// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/SInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "SInteractableInterface.h"
#include "ProfilingDebugging/CookStats.h"

// Console variable for showing debug info
static TAutoConsoleVariable<bool> CVarShowInteractionDebugInfo(
	TEXT("sar.ShowInteractionDebugInfo"), false, TEXT("Show debug information for interactions"), ECVF_Cheat);

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InteractionRadius = 250.0f;
}

// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
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
		// filter for interactable actors 
		TArray<FHitResult> FilteredHits = Hits.FilterByPredicate([](const FHitResult Hit)
		{
			const AActor* HitActor = Hit.GetActor();
			return HitActor && HitActor->Implements<USInteractableInterface>();
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
