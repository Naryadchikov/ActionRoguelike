// Fill out your copyright notice in the Description page of Project Settings.


#include "Demo/STargetDummy.h"

#include "Components/SAttributeComponent.h"

ASTargetDummy::ASTargetDummy()
{
	// Set up static mesh component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	// Set up attribute component
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
}

void ASTargetDummy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASTargetDummy::OnHealthChanged);
}

void ASTargetDummy::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewValue,
                                    float Delta)
{
	if (Delta < 0.0f)
	{
		MeshComp->SetScalarParameterValueOnMaterials(HitMaterialParameterName, GetWorld()->GetTimeSeconds());
	}
}
