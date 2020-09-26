// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSMannequin.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "../DebugTool/DebugLog.h"
#include "FPSPlayerComponent/FPSPlayerInput.h"
#include "FPSPlayerComponent/FPSPlayerAiming.h"
#include "../Public/FPSPlayerController.h"
#include "../Projectile/FPSStone.h"
#include "../FPSGame.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/ArrowComponent.h"
#include "Animation/AnimMontage.h"


#define OUT

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

	USceneComponent* RefMesh = Cast<USceneComponent>(GetMesh());
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
				PlayAnimMontage(FightAnimMontage, 1.5f, "Throw");
			SR_FightAnim(FName("Throw"), false);
			//Fire();
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
	PlayAnimMontage(FightAnimMontage, 1.5f, SlotName);

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



void AFPSMannequin::Fire()
{
	if (StoneBlueprinClass)
	{
		//UE_LOG(LogTemp, Error, TEXT("Rotation StoinPoint = %s"), *StoneSpawnPoint->GetComponentRotation().ToString());
		//StoneSpawnPoint->SetRelativeRotation(NewRotation);
		//LOG_S(FString::Printf((TEXT("Rotation StoinPoint = %s"), *StoneSpawnPoint->GetComponentRotation().ToString())));
		//UE_LOG(LogTemp, Warning, TEXT("Rotation StoinPoint = %s"), *StoneSpawnPoint->GetComponentRotation().ToString());

		FVector StoneLocation = StoneSpawnPoint->GetComponentLocation();  //GetMesh()->GetSocketLocation("StoneStartPoint");
		FRotator StoneRotation = StoneSpawnPoint->GetComponentRotation(); //GetMesh()->GetSocketRotation("StoneStartPoint");


		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = this;


		FVector NormalLocation = CalculateStoneVelocity();
		FRotator StoneForwardRotation = StoneSpawnPoint->GetForwardVector().Rotation();
		FRotator NormalRotation = NormalLocation.Rotation();
		FRotator DeltaRotation = NormalRotation - StoneForwardRotation;
		FRotator SpawnRotation = FRotator(DeltaRotation.Pitch, DeltaRotation.Yaw, 0);


		AFPSStone* Stone = GetWorld()->SpawnActor<AFPSStone>(StoneBlueprinClass, StoneLocation, StoneRotation/*StoneRotation*//*, ActorSpawnParams*/);

		UE_LOG(LogTemp, Warning, TEXT("ForwardVector StonePoint = %s"), *StoneSpawnPoint->GetForwardVector().ToString());
		UE_LOG(LogTemp, Warning, TEXT("ForwardVector Stone = %s"), *Stone->GetActorForwardVector().ToString());
		/*FVector NormalLocation = CalculateStoneVelocity();
		FRotator StoneForwardRotation = Stone->GetActorForwardVector().Rotation();
		FRotator NormalRotation = NormalLocation.Rotation();
		FRotator DeltaRotation = NormalRotation - StoneForwardRotation;*/

		//Stone->SetActorRelativeRotation(FRotator(DeltaRotation.Pitch, DeltaRotation.Yaw, 0));

		/*DrawDebugLine(
			GetWorld(),
			Stone->GetActorLocation(),
			Stone->GetActorForwardVector() * 100,
			FColor(255, 0, 0),
			false,
			10.f,
			0.f,
			5.f
		);*/

		//Stone->LaunchStone(CameraArm->GetComponentRotation().Vector() /*FVector(0, 0, MannequinInputComponent->GetZvalue())*/);
		LOG_S(FString("ProjectileStone"));
	}
}

FVector AFPSMannequin::CalculateStoneVelocity()
{

	FVector StartLocation = GetMesh()->GetSocketLocation("StoneStartPoint"); // CameraComponent->GetComponentLocation();
	//FRotator StartRotation = GetMesh()->GetSocketRotation("StonePoint"); // CameraComponent->GetComponentRotation();

	FVector EndLocation = MannequinAimingComponent->GetLineTraceEndPoint();
	
	/*DrawDebugLine(
		GetWorld(),
		StartLocation,
		EndLocation,
		FColor(255, 0, 0),
		false,
		0.f,
		0.f,
		5.f
	);*/

	//DrawDebugSphere(GetWorld(), StartLocation, 25, 12, FColor::Yellow, false, 2.f, 0, 1.f);
	//DrawDebugSphere(GetWorld(), EndLocation, 25, 12, FColor::Red, false, 20.f, 0, 3.f);
	LOG_S(FString::Printf(TEXT("ProjectileEndPoint = %s"), *EndLocation.ToString()));
	LOG_S(FString::Printf(TEXT("ProjectileStartPoint = %s"), *StartLocation.ToString()));
	//LOG_S(FString::Printf(TEXT("ProjectileShotDirection  = %s"), *ShotDirection.ToString()));
	//LOG_S(FString::Printf(TEXT("ProjectileRotation  = %s"), *StartRotation.ToString()));

	/*FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	bool LTSC = GetWorld()->LineTraceSingleByChannel(
		OUT Hit,
		OUT StartLocation,
		OUT EndLocation,
		COLLISION_AIM,
		QueryParams
	);*/


	FVector StartStoneLocation = StoneSpawnPoint->GetComponentLocation();//GetMesh()->GetSocketLocation("StoneStartPoint");
	FVector LaunchVelocity;

	UGameplayStatics::SuggestProjectileVelocity
	(
		this,
		OUT LaunchVelocity,
		StartStoneLocation,
		EndLocation,
		1000.f,
		false,
		0.f,
		0.f,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);

	DrawDebugSphere(GetWorld(), StartStoneLocation, 25, 12, FColor::Yellow, false, 10.f, 0, 1.f);
	DrawDebugSphere(GetWorld(), EndLocation, 25, 12, FColor::Red, false, 10.f, 0, 3.f);

	auto StoneDirection = LaunchVelocity.GetSafeNormal();

	return StoneDirection;
}

AFPSPlayerController* AFPSMannequin::GetSelfController()
{
	return Cast<AFPSPlayerController>(GetController());
}

// Called when the game starts or when spawned
void AFPSMannequin::BeginPlay()
{
	Super::BeginPlay();

	//LOG_F(GetCapsuleComponent()->GetScaledCapsuleRadius());

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


