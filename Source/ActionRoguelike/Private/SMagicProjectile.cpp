// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "Components/SphereComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	SphereComp->SetSphereRadius(20.0f);
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
