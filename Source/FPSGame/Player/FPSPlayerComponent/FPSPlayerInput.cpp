// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerInput.h"
#include "../FPSMannequin.h"
#include "../DebugTool/DebugLog.h"
#include "../../FPSGame.h"
#include "FPSPlayerAiming.h"
#include "../../Public/FPSPlayerController.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values for this component's properties
UFPSPlayerInput::UFPSPlayerInput()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
	SetIsReplicated(true);

}


// Called when the game starts
void UFPSPlayerInput::BeginPlay()
{
	Super::BeginPlay();
	Player = Cast<AFPSMannequin>(GetOwner());
	AimingComponent = GetOwner()->FindComponentByClass<UFPSPlayerAiming>();
	Player->SetPermissionMoving(true);
}




void UFPSPlayerInput::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//LOG_S(FString("Input TickComponent"));

	switch (FightState)
	{
	case EFightState::Aim:
		//LOG_S(FString("AimingState"));
		AimingComponent->AimPoint();
		break;
	case  EFightState::None:
		//LOG_S(FString("NonAimState"));
		break;
	default:
		break;
	}

}

void UFPSPlayerInput::SetupInputComponent(UInputComponent* Input)
{
	
	InputComponentt = Input;
	if (InputComponentt)
	{
		/// Bind Input axis
		Input->BindAxis("Turn", this, &UFPSPlayerInput::Azimuth);
		Input->BindAxis("LookUp", this, &UFPSPlayerInput::Elevation);
		Input->BindAxis("MoveForward", this, &UFPSPlayerInput::MoveForward);
		Input->BindAxis("MoveRight", this, &UFPSPlayerInput::MoveRight);
		Input->BindAction("Crouch", IE_Pressed, this, &UFPSPlayerInput::Crouch);
		Input->BindAction("Crouch", IE_Released, this, &UFPSPlayerInput::UnCrouch);
		Input->BindAction("Aiming", IE_Pressed, this, &UFPSPlayerInput::StartAiming);
		Input->BindAction("Aiming", IE_Released, this, &UFPSPlayerInput::UndoAiming);
		Input->BindAction("Fire", IE_Pressed, this, &UFPSPlayerInput::StartFire);
		UE_LOG(LogTemp, Error, TEXT("%s - has Component InputComponent..."), *GetOwner()->GetName())
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s - has not Component InputComponent !!!"), *GetOwner()->GetName())
	}
}

void UFPSPlayerInput::Azimuth(float Val)
{
	/// Old Code
	//float YawChange = Val * GetWorld()->GetDeltaSeconds() * 50;
	//auto NewRotation = Player->GetCameraSeatComponent()->RelativeRotation.Yaw + YawChange;
	//if (Player == nullptr) return;
	////Player->GetCameraSeatComponent()->AddLocalRotation(FRotator(Player->GetCameraSeatComponent()->GetComponentRotation().Pitch,NewRotation,0));
	//Player->GetCameraSeatComponent()->SetRelativeRotation(FRotator(Player->GetCameraSeatComponent()->RelativeRotation.Pitch,NewRotation,0));
	//LOG_S(FString("Mousex"));


	Player->AddControllerYawInput(Val * 50 * GetWorld()->GetDeltaSeconds());

}

void UFPSPlayerInput::Elevation(float Val)
{
	/// Old Code
	// We need it for Clamp Pitch
	float PitchChange = Val * GetWorld()->GetDeltaSeconds() * 50;
	if (Player == nullptr) return;
	//Player->GetCameraArmComponent()->AddLocalRotation(NewRotation);
	auto NewPitch = Player->GetCameraArmComponent()->RelativeRotation.Pitch + PitchChange;
	auto UpdatePitch = FMath::Clamp<float>(NewPitch, -30, 10);
	Player->GetCameraArmComponent()->SetRelativeRotation(FRotator(UpdatePitch,0,0));
	//LOG_S(FString::Printf(TEXT("Pitch = %f"), UpdatePitch));


	//Player->AddControllerPitchInput(Val * 45 * GetWorld()->GetDeltaSeconds());

		
}

void UFPSPlayerInput::MoveForward(float Val)
{
	if (Val != 0 && Player->GetPermissionMoving())
	{
	
		const FRotator Rotation = Player->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		Player->AddMovementInput(Direction, Val);
		//Player->SetPermissionMoving(Val);
		LOG_S(Direction.ToString());
	}
	


	//LOG_I(Player->bMoving);
}

void UFPSPlayerInput::MoveRight(float Val)
{
	if (Val != 0 && Player->GetPermissionMoving())
	{

		const FRotator Rotation = Player->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		Player->AddMovementInput(Direction, Val);
		//Player->SetPermissionMoving(Val);
	}
	
}

void UFPSPlayerInput::Crouch()
{
	Player->GetCharacterMovement()->MaxWalkSpeed = Player->GetDefaultCrouchSpeed();
	//Player->CanCrouched(true);
	Player->bIsCrouched = true;
	Player->SetPermissionCrouch(true);
	Player->ServerCrouch(true);
}

void UFPSPlayerInput::UnCrouch()
{
	Player->GetCharacterMovement()->MaxWalkSpeed = Player->GetDefalutMaxSpeed();
	//Player->CanCrouched(false);
	Player->bIsCrouched = false;
	Player->SetPermissionCrouch(false);
	Player->ServerCrouch(false);
}

void UFPSPlayerInput::StartAiming()
{
	FightState = EFightState::Aim;
	ApplyFightState(FightState);
}

void UFPSPlayerInput::UndoAiming()
{
	switch (FightState)
	{	
	case EFightState::Aim:
		FightState = EFightState::UndoAim;
		ApplyFightState(FightState);
		break;
	default:
		break;
	}
}

void UFPSPlayerInput::StartFire()
{
	FightState = EFightState::Fire;
	ApplyFightState(FightState);
}


void UFPSPlayerInput::ApplyFightState(EFightState State)
{
	
	switch (State)
	{
	case EFightState::None:
		break;
	case EFightState::Aim:
		ApplyAimState();
		break;
	case EFightState::UndoAim:
		ApplyUndoAimState();
		break;
	case EFightState::Fire:
		ApplyFireState();
		break;
	default:
		break;
	}
}


void UFPSPlayerInput::ApplyAimState()
{
	Player->bUseControllerRotationYaw = true;
	Player->SetPermissionMoving(false);
	Player->PlayFightAnim(FightState);
	LOG_I(Player->GetPermissionAiming());

	//AimingComponent->AimPoint();
}

void UFPSPlayerInput::ApplyUndoAimState()
{
	Player->bUseControllerRotationYaw = false;
	Player->SetPermissionMoving(true);
	Player->PlayFightAnim(FightState);

	if (AimingComponent->GetTraceEffect())
	{
		AimingComponent->DestroyTraceEffect();
	}
}

void UFPSPlayerInput::ApplyFireState()
{
	Player->Fire();
	Player->SetPermissionFire(true);


	Player->bUseControllerRotationYaw = false;
	Player->GetCameraArmComponent()->bUsePawnControlRotation = true;
	Player->SetPermissionMoving(false);

	if (AimingComponent->GetTraceEffect())
	{
		AimingComponent->DestroyTraceEffect();
	}

	FightState = EFightState::None;
}




