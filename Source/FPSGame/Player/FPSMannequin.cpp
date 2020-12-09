// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSMannequin.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "../DebugTool/DebugLog.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "FPSPlayerComponent/FPSPlayerAiming.h"
//#include "FPSPlayerComponent/FPSPlayerFireComponent.h"
#include "../Public/FPSPlayerController.h"
#include "../Projectile/FPSStone.h"
#include "../FPSGame.h"
#include "../Library/FPSMath.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Animation/AnimMontage.h"
#include "DrawDebugHelpers.h"
#include "Components/PawnNoiseEmitterComponent.h"


#define OUT

static int32 DebugPlayerRoleDrawing = 0;
FAutoConsoleVariableRef CVARDebugPlayerRoleDrawing(
	TEXT("FPS.DebugPlayerRole"),
	DebugPlayerRoleDrawing,
	TEXT("Draw Role Player Character"),
	ECVF_SetByConsole
);

// Sets default values
AFPSMannequin::AFPSMannequin(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*CameraSeat = CreateDefaultSubobject<USceneComponent>(TEXT("CameraSeat"));
	CameraSeat->SetupAttachment(GetMesh());
	CameraSeat->RelativeRotation = FRotator(0,90,0);
	CameraSeat->RelativeLocation = FVector(0,0, 200.0);*/
	
	bMoving = true;
	//AimTraceName = "AimBeamEnd";

	GetCapsuleComponent()->SetCapsuleRadius(15);
	GetMesh()->RelativeLocation = FVector(0, 0, -90);

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


	StoneSpawnPoint = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("ArrowPoint"));
	StoneSpawnPoint->SetupAttachment(GetMesh());
	StoneSpawnPoint->RelativeLocation = FVector(-59, 49, 150);
	StoneSpawnPoint->RelativeRotation = FRotator(0, 0, 90);
	StoneSpawnPoint->SetIsReplicated(true);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	HeadCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("HeadCollision"));
	HeadCollision->SetupAttachment(GetMesh());
	HeadCollision->RelativeLocation = FVector(13.f, 0.f, 175.f);
	HeadCollision->SetCapsuleHalfHeight(35.f);
	HeadCollision->SetCapsuleRadius(27.f);
	HeadCollision->SetCollisionProfileName(TEXT("CharacterHead"));
	//HeadCollision->OnComponentHit.AddDynamic(this, &AFPSMannequin::HeadShoot);

	TM_ShotDirection.Add(0, "Forward");
	//TM_ShotDirection.Add(90, "Left");
	TM_ShotDirection.Add(180, "Back");
	//TM_ShotDirection.Add(270, "Right");

	GetCharacterMovement()->bOrientRotationToMovement = true;
	//GetCharacterMovement()->MaxWalkSpeed = OwnFeatures.MaxSpeed;

	MannequinAimingComponent = CreateDefaultSubobject<UFPSPlayerAiming>(TEXT("MannequinAimingComonent"));
	MannequinInputComponent = CreateDefaultSubobject<UFPSPlayerInput>(TEXT("MannequinInputComonent"));
	MannequinFireComponent = CreateDefaultSubobject<UFPSPlayerFireComponent>(TEXT("MannequinFireComonent"));
	MannequinNoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("MannequinNoiseEmitterComponent"));


	Tags.Add(FName("Player"));

	bReplicates = true;
	bReplicateMovement = true;


}




// Called when the game starts or when spawned
void AFPSMannequin::BeginPlay()
{
	Super::BeginPlay();

	HeadCollision->OnComponentHit.AddDynamic(this, &AFPSMannequin::HeadShoot);


	SetBeginPlayParams();
	
	if (Role == ROLE_Authority)
	{
		LOG_S(FString("Server"));
	}
	if (Role == ROLE_AutonomousProxy)
	{
		LOG_S(FString("Client"));
	}
}


void AFPSMannequin::SetBeginPlayParams()
{
	if (Role < ROLE_Authority)
		SR_SetBeginPlayParams();

	
	OwnController = GetSelfController();
	if (OwnController)
	{
		StoneSpread = OwnController->GetMaxAimPrecision() - OwnFeatures.AimPrecision;
		Loudness = OwnController->GetMaxLoud() - OwnFeatures.Smart;
		Solidity = OwnController->GetMaxStrenght() - OwnFeatures.Strength;
		bCrouch = false;
	}
	else
	{
		Solidity = 10 - OwnFeatures.Strength;
		Loudness = 1 - OwnFeatures.Smart;
		StoneSpread = 10 - OwnFeatures.AimPrecision;
	}
	

}


void AFPSMannequin::SR_SetBeginPlayParams_Implementation()
{
	SetBeginPlayParams();
	LOG_S(FString("SR_SetBeginPlayParams_Implementation"))
}

bool AFPSMannequin::SR_SetBeginPlayParams_Validate()
{
	return true;
}

void AFPSMannequin::HeadShoot(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if (!OtherActor)
		return;
	FName* StoneTag = OtherActor->Tags.GetData();
	FString ST = StoneTag->GetPlainNameString();
	if (ST == "Projectile")
	{
		float HitDegree = FPSMath::GetHitPointDirectionYaxis(Hit.ImpactPoint, HeadCollision->GetComponentLocation(), this);
		LOG_S(FString::Printf(TEXT("Head Hit Stone HitDegree = %f"), HitDegree));
		HeadShotDirection = HitDegree;
		bIsheadshot = true;
		bMoving = false;

		FName AnimSectionName = TM_ShotDirection[HitDegree];
		LOG_S(FString::Printf(TEXT("Head Hit Stone Solidity_1 = %f"), Solidity));
		int32 sectionIndex = HeadShotAnim->GetSectionIndex(AnimSectionName);
		PlayAnimMontage(HeadShotAnim, 1.f, TM_ShotDirection[HitDegree]);

		Solidity += HeadShotAnim->GetSectionLength(sectionIndex);
		LOG_S(FString::Printf(TEXT("Head Hit Stone HeadShot Anim Lenght = %f"), HeadShotAnim->GetSectionLength(sectionIndex)));
		LOG_S(FString::Printf(TEXT("Head Hit Stone Solidity_2 = %f"), Solidity));

		FName AnimSlotname;
		if (HitDegree == 0)
		{
			AnimSlotname = FName("GetUp_F");
		}
		else
		{
			if (HeadShotAnim->IsValidSectionName(FName("GetUp_B")))
			{
				AnimSlotname = FName("GetUp_B");
			}
			else
			{
				AnimSlotname = FName("GetUp_F");
			}
		}
		TimerDel.BindUFunction(this, FName("GetUp"), AnimSlotname);
		GetWorld()->GetTimerManager().SetTimer(FTimer_HeadShotAnim, TimerDel, Solidity, false);
	}
	
}



void AFPSMannequin::GetUp(FName slotname)
{
	PlayAnimMontage(HeadShotAnim, 1.f, slotname);
	LOG_S(FString("Head Hit Stone GetUp"));
}

// Called every frame
void AFPSMannequin::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//LOG_F(GetCharacterMovement()->GetMaxSpeed());

	//LOG_S(FString::Printf(TEXT("PermisionAiming = %i"), bAiming));

	if (Role == ROLE_AutonomousProxy && ClientRandomFireRotate)
	{
		SetActorRotation(RandomPointRotation);
	}

	if (DebugPlayerRoleDrawing > 0)
	{
		if (Role == ROLE_Authority)
			DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 200), FString("Server"), nullptr, FColor::Red, 10.f);
		if (Role == ROLE_AutonomousProxy)
			DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 200), FString("Client"), nullptr, FColor::Yellow, 10.f);
	}

}



void AFPSMannequin::UnPossessed()
{
	Super::UnPossessed();

	Destroy(true, true);
	UE_LOG(LogTemp, Warning, TEXT("UNPOSSESSED function"));
}

void AFPSMannequin::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSMannequin, bCrouch);
	DOREPLIFETIME(AFPSMannequin, bMoving);
	DOREPLIFETIME(AFPSMannequin, bAiming);
	DOREPLIFETIME(AFPSMannequin, Loudness);
	DOREPLIFETIME(AFPSMannequin, OwnFeatures);
	//DOREPLIFETIME(AFPSMannequin, FightAnimMontage);
	DOREPLIFETIME(AFPSMannequin, FireAnimPlayRate);
	DOREPLIFETIME(AFPSMannequin, RandomPointRotation);
	DOREPLIFETIME(AFPSMannequin, StoneSpread);

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




void AFPSMannequin::SetPermissionCrouch(bool val)
{
	if(Role < ROLE_Authority)
		SR_Crouch(val);

	bCrouch = val;
	if (val)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetDefaultCrouchSpeed();
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = GetDefalutMaxSpeed();
	}

}


void AFPSMannequin::SR_Crouch_Implementation(bool UpdateCrouch)
{
	SetPermissionCrouch(UpdateCrouch);
}



bool AFPSMannequin::SR_Crouch_Validate(bool UpdateCrouch)
{
	return true;
}



void AFPSMannequin::SR_MakeStepNoise_Implementation(bool enable)
{
	if (enable)
	{
		MannequinNoiseEmitterComponent->MakeNoise(this, Loudness, GetActorLocation());
		LOG_S(FString::Printf(TEXT("Player Noise = %f"), Loudness));
	}
	else
	{
		LOG_S(FString("Player Silence"));
	}
}

bool AFPSMannequin::SR_MakeStepNoise_Validate(bool enable)
{
	return true;
}

void AFPSMannequin::PlayFightAnim(EFightState State)
{
	MannequinFireComponent->PlayAnim(State);
}



void AFPSMannequin::SR_RotateCroshairDirection_Implementation(FRotator Direction)
{
	/*RandomPointRotation = Direction;*/
	RandomPointRotation = Direction;
	SetActorRotation(Direction);
	//RandomPointRotation = Direction;
	LOG_S(FString::Printf(TEXT("RRR Server Rotation = %s"), *Direction.ToString()));
}

bool AFPSMannequin::SR_RotateCroshairDirection_Validate(FRotator Direction)
{
	return true;
}




void AFPSMannequin::SR_FightAnim_Implementation(const FName& SlotName, bool bEnabelPlayerYaw)
{
	/// RPC is also executed for HostPlayer
	if (IsLocallyControlled() && Role == ROLE_Authority)
	{
		LOG_S(FString("Server Executed"));
	}
	bUseControllerRotationYaw = bEnabelPlayerYaw;
	PlayAnimMontage(MannequinFireComponent->GetFightAnim(), FireAnimPlayRate, SlotName);

	/// Multi cast Replicated the Animation for all Client
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
		PlayAnimMontage(MannequinFireComponent->GetFightAnim(), FireAnimPlayRate, SlotName);
	}
		
}



void AFPSMannequin::SetPermissionAiming(bool val)
{
	SR_UpdateAiming(val);
	bAiming = val; 
	LOG_S(FString::Printf(TEXT("Aiming Val = %i"), bAiming));

}

void AFPSMannequin::SR_UpdateAiming_Implementation(bool val)
{
	bAiming = val;
}

bool AFPSMannequin::SR_UpdateAiming_Validate(bool val)
{
	// TODO more clear
	return true;
}

void AFPSMannequin::Fire()
{
	if (Role == ROLE_Authority)
	{
		LOG_S(FString::Printf(TEXT("PermisionAiming Server = %i"), bAiming));
	}
	else
	{
		LOG_S(FString::Printf(TEXT("PermisionAiming Client = %i"), bAiming));
	}

	SR_Fire();

}



void AFPSMannequin::SR_Fire_Implementation()
{
	MannequinFireComponent->FireStone();
}


bool AFPSMannequin::SR_Fire_Validate()
{
	return true;
}





AFPSPlayerController* AFPSMannequin::GetSelfController()
{
	return Cast<AFPSPlayerController>(GetController());
}






// ====================================================================================== //
//                      Remove to FireComponent

//void AFPSMannequin::PlayFightAnim(EFightState State)
//{
//
//	if (FightAnimMontage)
//	{
//
//		switch (State)
//		{
//		case EFightState::Aim:
//			// As RPC is executed on Server and
//			// As RPC is also called from HostPlayer and what Server Know is also is known HostPlayer
//			// We need not PlayAnimMontage() is Executed two times for HostPlayer
//			if(Role < ROLE_Authority)
//				PlayAnimMontage(FightAnimMontage, 1.f, "StartThrow");
//			// This executed only server not Client 
//			SR_FightAnim(FName("StartThrow"), true);
//			LOG_S(FString("From Server To Client"));
//			break;
//		case EFightState::UndoAim:
//			if (Role < ROLE_Authority)
//				PlayAnimMontage(FightAnimMontage, 1.f, "UndoThrow");
//			SR_FightAnim(FName("UndoThrow"), false);
//			break;
//		case EFightState::Fire:
//
//			if (!bAiming)
//			{
//				ClientRandomFireRotate = true;
//				FRotator CroshairRotation = GetControlRotation();
//				SR_RotateCroshairDirection(CroshairRotation);
//			}
//
//			if (Role < ROLE_Authority)
//				PlayAnimMontage(FightAnimMontage, FireAnimPlayRate, "Throw");
//			SR_FightAnim(FName("Throw"), false);
//			//Fire();
//			break;
//		case EFightState::None:
//			break;
//		default:
//			break;
//		}
//
//	}
//
//	//MannequinFireComponent->PlayAnim(State);
//}


//void AFPSMannequin::SR_Fire_Implementation()
//{
//
//	if (StoneBlueprinClass)
//	{
//
//		FVector StoneSpawnLocation;
//		FVector StoneEndLocation;
//		float StoneSpeed;
//		FRotator StoneRotation;
//		LOG_S(FString("SR_Fire_Implementation"));
//		if (bAiming)
//		{
//			// FVector StoneLocation = GetMesh()->GetSocketLocation("StoneStartPoint"); -- 
//				// In Socket location case Stone spawn and SuggestProjectileVelocity() not work properly
//				// Fire location don't match Aim location
//
//			/// Get Location & Speed
//			StoneSpawnLocation = StoneSpawnPoint->GetComponentLocation();
//			StoneEndLocation = MannequinAimingComponent->GetLineTraceEndPoint();
//			StoneSpeed = MannequinAimingComponent->GetLineTraceLength() / 2;
//			/*LOG_S(FString("TTT SR_Fire_Implementation"));
//			LOG_S(FString::Printf(TEXT("TTT PermisionAiming StoneSpawnLocation = %s"), *StoneSpawnLocation.ToString()));
//			LOG_S(FString::Printf(TEXT("TTT PermisionAiming StoneEndLocation = %s"), *StoneEndLocation.ToString()));
//			LOG_S(FString::Printf(TEXT("TTT PermisionAiming Speed = %f"), StoneSpeed));*/
//			
//
//			/// SugestProjectileVelocity
//			FVector DirectionLocation = MannequinFireComponent->GetFireDirection(StoneSpawnLocation, StoneEndLocation, StoneSpeed);
//			/*LOG_S(FString::Printf(TEXT("TTT PermisionAiming DirectionLocation = %s"), *DirectionLocation.ToString()));
//			LOG_S(FString("TTT ======================================================= "));*/
//			/// Spread of Stone
//			float HalfRad = FMath::DegreesToRadians(BulletSpread);
//			DirectionLocation = FMath::VRandCone(DirectionLocation, HalfRad, HalfRad);
//			LOG_F(BulletSpread);
//
//			/// Get Stone Direction
//			FVector NormalLocation = DirectionLocation.GetSafeNormal();
//			StoneRotation = NormalLocation.Rotation();
//
//
//			/*LOG_S(FString::Printf(TEXT("FFF PermisionAiming StoneRotation = %s"), *StoneRotation.ToString()));
//			LOG_S(FString::Printf(TEXT("FFF PermisionAiming Speed = %f"), StoneSpeed));
//
//			
//			LOG_S(FString("PermisionAiming Spawn_AimPoint"));*/
//			if (Role == ROLE_Authority)
//			{
//				LOG_S(FString("FFF PermisionAiming Server"));
//			}
//			if (Role == ROLE_AutonomousProxy)
//			{
//				LOG_S(FString("FFF PermisionAiming Client"));
//			}
//
//			LOG_S(FString("AAA Fire_ Aiming"));
//			
//		}
//		else
//		{
//			StoneSpawnLocation = StoneSpawnPoint->GetComponentLocation();
//			float PitchVal = FMath::RandRange(1.f, 3.f);
//			StoneRotation = FRotator(PitchVal, GetActorRotation().Yaw, 0);
//
//			float speedrange = FMath::RandRange(2.f, 4.f);
//			StoneSpeed = MannequinAimingComponent->GetLineTraceLength() / speedrange;
//			LOG_S(FString("AAA Fire_ Random"));
//		}
//
//		//Set Spawn Collision Handling Override
//		FActorSpawnParameters ActorSpawnParams;
//		//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
//		ActorSpawnParams.Instigator = this;
//		
//		
//		
//		AFPSStone* Stone = GetWorld()->SpawnActor<AFPSStone>(StoneBlueprinClass, StoneSpawnLocation, StoneRotation, ActorSpawnParams);
//		if (Stone)
//		{
//			LOG_S(FString::Printf(TEXT("SSS Stone Scale = %s"), *Stone->GetActorScale().ToString()));
//			LOG_S(FString::Printf(TEXT("SSS PermisionAiming StoneSpawnLocation = %s"), *StoneSpawnLocation.ToString()));
//			Stone->LaunchStone(StoneSpeed);
//			DrawDebugLine(
//				GetWorld(),
//				Stone->GetActorLocation(),
//				Stone->GetActorLocation() + Stone->GetActorForwardVector() * 200.f,
//				FColor(0, 255, 0),
//				false,
//				10.f,
//				0.f,
//				5.f
//			);
//		}
//
//	}
//
//	//MannequinFireComponent->FireStone();
//}