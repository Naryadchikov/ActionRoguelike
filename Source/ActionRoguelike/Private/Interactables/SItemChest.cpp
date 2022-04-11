// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactables/SItemChest.h"
#include "Components/StaticMeshComponent.h"

ASItemChest::ASItemChest()
{
	// Set up collision component
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>("BaseMesh");
	RootComponent = BaseMesh;

	// Set up effect component
	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>("LidMesh");
	LidMesh->SetupAttachment(RootComponent);
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	UE_LOG(LogTemp, Log, TEXT("Empty default 'Interact' implementation from C++"));
}
