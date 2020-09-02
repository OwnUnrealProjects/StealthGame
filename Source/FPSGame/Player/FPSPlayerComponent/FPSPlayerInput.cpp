// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerInput.h"
#include "../FPSMannequin.h"
#include "../DebugTool/DebugLog.h"
#include "../../FPSGame.h"
#include "FPSPlayerAiming.h"

#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"


// Sets default values for this component's properties
UFPSPlayerInput::UFPSPlayerInput()
{
	SetIsReplicated(true);

}


// Called when the game starts
void UFPSPlayerInput::BeginPlay()
{
	Super::BeginPlay();

	AimingComponent = GetOwner()->FindComponentByClass<UFPSPlayerAiming>();
}


void UFPSPlayerInput::SetupInputComponent(AFPSMannequin* OwnerPlayer, UInputComponent* Input)
{
	Player = OwnerPlayer;
	InputComponentt = Input;
	if (InputComponentt)
	{
		/// Bind Input axis
		InputComponentt->BindAxis("Turn", this, &UFPSPlayerInput::Azimuth);
		InputComponentt->BindAxis("LookUp", this, &UFPSPlayerInput::Elevation);
		InputComponentt->BindAxis("MoveForward", this, &UFPSPlayerInput::MoveForward);
		InputComponentt->BindAxis("MoveRight", this, &UFPSPlayerInput::MoveRight);
		InputComponentt->BindAction("Crouch", IE_Pressed, this, &UFPSPlayerInput::Crouch);
		InputComponentt->BindAction("Crouch", IE_Released, this, &UFPSPlayerInput::UnCrouch);
		InputComponentt->BindAction("Aiming", IE_Pressed, this, &UFPSPlayerInput::StartAiming);
		InputComponentt->BindAction("Aiming", IE_Released, this, &UFPSPlayerInput::UndoAiming);
		InputComponentt->BindAction("Fire", IE_Pressed, this, &UFPSPlayerInput::StartFire);
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
	//auto Yaw = Val * GetWorld()->GetDeltaSeconds() * 45;
	//auto NewRotation = FRotator(0.f, Yaw, 0.f);
	//if (Player == nullptr) return;
	//Player->GetCameraSeatComponent()->AddLocalRotation(NewRotation);
	//LOG_S(FString("Mousex"));

	Player->AddControllerYawInput(Val * 45 * GetWorld()->GetDeltaSeconds());

}

void UFPSPlayerInput::Elevation(float Val)
{
	/// Old Code
	//float PitchChange = Val * GetWorld()->GetDeltaSeconds() * 50;
	//if (Player == nullptr) return;
	////Player->GetCameraArmComponent()->AddLocalRotation(NewRotation);
	//auto NewPitch = Player->GetCameraArmComponent()->RelativeRotation.Pitch + PitchChange;
	//auto UpdatePitch = FMath::Clamp<float>(NewPitch, -30, 10);
	//Player->GetCameraArmComponent()->SetRelativeRotation(FRotator(UpdatePitch,0,0));
	//LOG_S(FString::Printf(TEXT("Pitch = %f"), UpdatePitch));



	Player->AddControllerPitchInput(Val * 45 * GetWorld()->GetDeltaSeconds());
		
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
	CanAiming(true);
}

void UFPSPlayerInput::UndoAiming()
{

	CanAiming(false);
}

void UFPSPlayerInput::CanAiming(bool Val)
{
	if (Val)
	{
		FireState = EFireState::Aim;

		Player->bUseControllerRotationYaw = true;
		Player->GetCameraArmComponent()->bUsePawnControlRotation = false;
		Player->SetPermissionMoving(false);
		Player->PlayAimingAnim(FireState);
		LOG_I(Player->GetPermissionAiming());

		AimingComponent->AimPoint();

		FireState = EFireState::UndoAim;
	}
	else
	{
		Player->bUseControllerRotationYaw = false;
		Player->GetCameraArmComponent()->bUsePawnControlRotation = true;
		Player->SetPermissionMoving(true);
		Player->PlayAimingAnim(FireState);

		if (AimingComponent->GetTraceEffect())
			AimingComponent->GetTraceEffect()->Deactivate();
	}
}


void UFPSPlayerInput::StartFire()
{
	FireState = EFireState::Fire;

	Player->Fire();
	Player->SetPermissionFire(true);


	Player->bUseControllerRotationYaw = false;
	Player->GetCameraArmComponent()->bUsePawnControlRotation = true;
	Player->SetPermissionMoving(false);

	if (AimingComponent->GetTraceEffect())
		AimingComponent->GetTraceEffect()->Deactivate();
}

