// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSMannequin.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "../DebugTool/DebugLog.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "FPSPlayerComponent/FPSPlayerAiming.h"
#include "../Public/FPSPlayerController.h"

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
	CameraArm->bInheritPitch = false;
	CameraArm->bInheritYaw = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraArm);
	CameraComponent->RelativeLocation = FVector(0, 100, 0);
	CameraComponent->RelativeRotation = FRotator(-15,-10,0);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	MannequinAimingComponent = CreateDefaultSubobject<UFPSPlayerAiming>(TEXT("MannequinAimingComonent"));
	MannequinInputComponent = CreateDefaultSubobject<UFPSPlayerInput>(TEXT("MannequinInputComonent"));

	bReplicates = true;
	bReplicateMovement = true;

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

void AFPSMannequin::PlayFightAnim(EFightState State)
{
	
	if (FightAnimMontage)
	{

		switch (State)
		{
		case EFightState::Aim:
			bAiming = true;
			// As RPC is executed on Server and
			// As RPC is also called from HostPlayer and what Server Know is also is known HostPlayer
			// We need not PlayAnimMontage() is Executed two times for HostPlayer
			if(Role < ROLE_Authority)
				PlayAnimMontage(FightAnimMontage, 1.f, "StartThrow");
			SR_FightAnim(FName("StartThrow"), true);
			LOG_S(FString("From Server To Client"));
			break;
		case EFightState::UndoAim:
			bAiming = false;
			if (Role < ROLE_Authority)
				PlayAnimMontage(FightAnimMontage, 1.f, "UndoThrow");
			SR_FightAnim(FName("UndoThrow"), false);
			break;
		case EFightState::Fire:
			bAiming = false;
			if (Role < ROLE_Authority)
				PlayAnimMontage(FightAnimMontage, 1.f, "Throw");
			SR_FightAnim(FName("Throw"), false);
			break;
		case EFightState::None:
			break;
		default:
			break;
		}

	}
}

void AFPSMannequin::SR_FightAnim_Implementation(const FName& SlotName, bool bEnabelPlayerYaw)
{
	/// RPC is also executed for HostPlayer
	if (IsLocallyControlled() && Role == ROLE_Authority)
	{
		LOG_S(FString("Server Executed"));
	}
	bUseControllerRotationYaw = bEnabelPlayerYaw;
	PlayAnimMontage(FightAnimMontage, 1.f, SlotName);
	/// Multicast Replicated the Animation for all Client
	MC_FightAnim(SlotName);
	//LOG_S(FString("Server Executed"));
}

bool AFPSMannequin::SR_FightAnim_Validate(const FName& SlotName, bool bEnabelPlayerYaw)
{

	return true;
}

void AFPSMannequin::MC_FightAnim_Implementation(const FName& SlotName)
{
	
	if (!IsLocallyControlled())
	{
		LOG_S(FString("NetMulticast Executed"));
		PlayAnimMontage(FightAnimMontage, 1.f, SlotName);
	}
		
}



AFPSPlayerController* AFPSMannequin::GetSelfController()
{
	return Cast<AFPSPlayerController>(GetController());
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
	DOREPLIFETIME(AFPSMannequin, FightAnimMontage);

}


// Called to bind functionality to input
void AFPSMannequin::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	if (IsLocallyControlled())
	{
		MannequinInputComponent->SetupInputComponent(PlayerInputComponent);
		//LOG_S(FString("SetupInput -- MovementInput..........................."));
	}
	
}


