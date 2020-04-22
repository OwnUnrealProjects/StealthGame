// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"

#include "FPSProjectile.h"
#include "FPSObjectiveActor.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "Components/PawnNoiseEmitterComponent.h"



AFPSCharacter::AFPSCharacter()
{
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->RelativeLocation = FVector(0, 0, BaseEyeHeight); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(CameraComponent);
	Mesh1PComponent->CastShadow = false;
	Mesh1PComponent->RelativeRotation = FRotator(2.0f, -15.0f, 5.0f);
	Mesh1PComponent->RelativeLocation = FVector(0, 0, -160.0f);

	// Create a gun mesh component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMeshComponent->CastShadow = false;
	GunMeshComponent->SetupAttachment(Mesh1PComponent, "GripPoint");

	NoiseEmitterComp = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));

	bReplicates = true;
}


void AFPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//DOREPLIFETIME(AFPSCharacter, bIsCarryingObjective);
	//DOREPLIFETIME_CONDITION(AFPSCharacter, bIsCarryingObjective, COND_ReplayOnly);
	DOREPLIFETIME_CONDITION(AFPSCharacter, bIsCarryingObjective, COND_OwnerOnly);
	DOREPLIFETIME(AFPSCharacter, bGuardSeen);
	DOREPLIFETIME(AFPSCharacter, bReturnObjective);
}


void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up Gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

}

//void AFPSCharacter::Landed(const FHitResult & Hit)
//{
//	UE_LOG(LogTemp, Warning, TEXT("Player Landed"));
//	MakeNoise(10, this, GetActorLocation(), 10000.f);
//	//PawnMakeNoise(100.f, GetActorLocation(), true, this);
//
//}

FString AFPSCharacter::GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}



void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsLocallyControlled())
	{
		FRotator NewRotatoin = CameraComponent->RelativeRotation; // Mesh1PComponent
		NewRotatoin.Pitch = RemoteViewPitch * 360.f / 250.f;

		CameraComponent->SetRelativeRotation(NewRotatoin);

		
	}
	
	
	DrawDebugString(GetWorld(), FVector(200, 0, 100.f), GetEnumText(Role), this, FColor::Red, DeltaTime);
	DrawDebugString(GetWorld(), FVector(200, 0, 150.f), GetName(), this, FColor::White, DeltaTime);
	DrawDebugString(GetWorld(), FVector(200, 0, 200.f), FString::Printf(TEXT("CarryingObjective = %i"), bIsCarryingObjective), this, FColor::Red, DeltaTime);

}


void AFPSCharacter::Fire()
{
	
	ServerFire();

	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
		}
	}
}


void AFPSCharacter::ServerFire_Implementation()
{
	
	// try and fire a projectile
	if (ProjectileClass)
	{

		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = GunMeshComponent->GetSocketRotation("Muzzle");

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = this;

		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
	}
}

bool AFPSCharacter::ServerFire_Validate()
{
	return true;
}

void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::UnPossessed()
{

	//Super::UnPossessed();
	Destroy(true, true);
	UE_LOG(LogTemp, Warning, TEXT("UNPOSSESSED function"));

	if (bReturnObjective)
	{
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ActorSpawnParams.Instigator = this;

		auto Objective = GetWorld()->SpawnActor<AFPSObjectiveActor>(BPObjective, GetActorLocation(), FRotator::ZeroRotator, ActorSpawnParams);
		if (Objective)
		{
			UE_LOG(LogTemp, Warning, TEXT("SCharacter --- Objective Name = %s"), *Objective->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SCharacter ---  Objective is NULL"));
		}
	}

}
