// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SProjectileBase.h"
#include "SMagicProjectile.h"
#include "Projectiles/SDashProjectile.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/SAttributeComponent.h"
#include "Components/SInteractionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set up camera and spring arm components
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	// Set up movement
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Set up attribute component
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	// Set up interaction component
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	// Set up primary attack execution delay
	PrimaryAttackExecutionDelay = 0.2f;

	// Set up primary attack execution delay
	SecondaryAttackExecutionDelay = 0.2f;

	// Set up primary attack execution delay
	DashExecutionDelay = 0.2f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Clear All timers that belong to this actor.
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	const FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	if (PrimaryAttackAnim)
	{
		if (!GetWorldTimerManager().IsTimerActive(TimerHandle_PrimaryAttack))
		{
			PlayAnimMontage(PrimaryAttackAnim);
			GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::SpawnPrimaryAttackProjectile,
			                                PrimaryAttackExecutionDelay);
		}
	}
	else
	{
		SpawnPrimaryAttackProjectile();
	}
}

void ASCharacter::SpawnPrimaryAttackProjectile()
{
	SpawnProjectile(PrimaryProjectileClass);
}

void ASCharacter::BlackHoleAttack()
{
	if (SecondaryAttackAnim)
	{
		if (!GetWorldTimerManager().IsTimerActive(TimerHandle_SecondaryAttack))
		{
			PlayAnimMontage(SecondaryAttackAnim);
			GetWorldTimerManager().SetTimer(TimerHandle_SecondaryAttack, this, &ASCharacter::SpawnBlackHoleProjectile,
			                                SecondaryAttackExecutionDelay);
		}
	}
	else
	{
		SpawnBlackHoleProjectile();
	}
}

void ASCharacter::SpawnBlackHoleProjectile()
{
	SpawnProjectile(SecondaryProjectileClass);
}

void ASCharacter::Dash()
{
	if (DashAnim)
	{
		if (!GetWorldTimerManager().IsTimerActive(TimerHandle_Dash))
		{
			PlayAnimMontage(SecondaryAttackAnim);
			GetWorldTimerManager().SetTimer(TimerHandle_Dash, this, &ASCharacter::SpawnDashProjectile,
			                                DashExecutionDelay);
		}
	}
	else
	{
		SpawnDashProjectile();
	}
}

void ASCharacter::SpawnDashProjectile()
{
	SpawnProjectile(DashProjectileClass);
}

void ASCharacter::SpawnProjectile(TSubclassOf<ASProjectileBase> ClassToSpawn)
{
	if (ensureAlways(ClassToSpawn))
	{
		// find attack spawn location, which is the hand socket
		const FVector SpawnLocation = [this]()
		{
			if (IsValid(GetMesh()) && GetMesh()->DoesSocketExist(ProjectileSpawnSocketName))
			{
				return GetMesh()->GetSocketLocation(ProjectileSpawnSocketName);
			}
			UE_LOG(LogTemp, Warning, TEXT("Mesh is invalid or socket with 'ProjectileSpawnSocketName' does not exist"));
			return GetActorLocation();
		}();

		// find spawn rotation by line-tracing from camera to the world, finding desired 'impact' location
		const FVector Start = CameraComp->GetComponentLocation();
		const FVector End = Start + GetControlRotation().Vector() * 5000.f;

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_PhysicsBody);
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(this);

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
		SpawnParams.Instigator = this;

		GetWorld()->SpawnActor<ASProjectileBase>(ClassToSpawn, SpawnTM, SpawnParams);
	}
}

void ASCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Rotation Debug Visualization -- //
	constexpr float DrawScale = 100.0f;
	constexpr float Thickness = 5.0f;

	// Offset to the right of pawn
	const FVector LineStart = GetActorLocation() + GetActorRightVector() * 100.0f;

	// Set line end in direction of the actor's forward
	const FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);

	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0,
	                          Thickness);

	// Set line end in direction of the controller rotation
	const FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);

	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f,
	                          0, Thickness);
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJumping);

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::BlackHoleAttack);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &ASCharacter::Dash);

	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
}
