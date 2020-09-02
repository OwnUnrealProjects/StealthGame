// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSMannequin.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "../DebugTool/DebugLog.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "FPSPlayerComponent/FPSPlayerAiming.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "Animation/AnimMontage.h"


// Sets default values
AFPSMannequin::AFPSMannequin()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*CameraSeat = CreateDefaultSubobject<USceneComponent>(TEXT("CameraSeat"));
	CameraSeat->SetupAttachment(GetMesh());
	CameraSeat->RelativeRotation = FRotator(0,90,0);
	CameraSeat->RelativeLocation = FVector(0,0, 200.0);*/
	
	bMoving = true;
	//AimTraceName = "AimBeamEnd";

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(GetMesh());
	CameraArm->RelativeRotation = FRotator(0, 90, 0);
	CameraArm->RelativeLocation = FVector(0, 0, 200.0);
	CameraArm->TargetArmLength = 300;
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->bInheritPitch = true;
	CameraArm->bInheritYaw = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraArm);
	CameraComponent->RelativeRotation = FRotator(-15,0,0);
	
	bUseControllerRotationPitch = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	MannequinInputComponent = CreateDefaultSubobject<UFPSPlayerInput>(TEXT("MannequinInputComonent"));
	MannequinAimingComponent = CreateDefaultSubobject<UFPSPlayerAiming>(TEXT("MannequinAimingComonent"));

	bReplicates = true;

}

void AFPSMannequin::ServerCrouch_Implementation(bool UpdateCrouch)
{
	bCrouch = UpdateCrouch;
	if (UpdateCrouch)
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultCrouchSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = DefaultMaxSpeed;
	}
}

bool AFPSMannequin::ServerCrouch_Validate(bool UpdateCrouch)
{
	return true;
}

void AFPSMannequin::PlayAimingAnim(EFireState State)
{
	
	if (FireAnimation)
	{

		switch (State)
		{
		case EFireState::Aim:
			bAiming = true;
			PlayAnimMontage(FireAnimation, 1.f, "StartThrow");
			break;
		case EFireState::UndoAim:
			bAiming = false;
			PlayAnimMontage(FireAnimation, 1.f, "UndoThrow");
			break;
		case EFireState::Fire:
		case EFireState::None:
			break;
		default:
			break;
		}

	}
}

void AFPSMannequin::Fire()
{
	PlayAnimMontage(FireAnimation, 1.f, "Throw");

}

// Called when the game starts or when spawned
void AFPSMannequin::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AFPSMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//LOG_F(GetCharacterMovement()->GetMaxSpeed());
}


void AFPSMannequin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSMannequin, bCrouch);
	DOREPLIFETIME(AFPSMannequin, bMoving);

}


// Called to bind functionality to input
void AFPSMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (IsLocallyControlled())
	{
		MannequinInputComponent->SetupInputComponent(this, PlayerInputComponent);
		//LOG_S(FString("SetupInput -- MovementInput..........................."));
	}
	
}


