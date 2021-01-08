// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerJump.h"
#include "../FPSMannequin.h"
#include "../../DebugTool/DebugLog.h"
#include "../../Public/FPSPlayerState.h"

#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

#define OUT

// Sets default values for this component's properties
UFPSPlayerJump::UFPSPlayerJump()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	bReplicates = true;
}




// Called when the game starts
void UFPSPlayerJump::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AFPSMannequin>(GetOwner());
}


// Called every frame
void UFPSPlayerJump::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	IsCharacterJump = Player->GetCharacterMovement()->IsFalling();
	if (IsCharacterJump && IsHeigthLedge)
	{
		if (Player->Role == ROLE_Authority)
			LOG_S(FString::Printf(TEXT("Server ActorLocation = %s"), *Player->GetActorLocation().ToString()));
		if (Player->Role == ROLE_AutonomousProxy)
			LOG_S(FString::Printf(TEXT("Clinet ActorLocation = %s"), *Player->GetActorLocation().ToString()));
		if (Player->Role == ROLE_SimulatedProxy)
			LOG_S(FString::Printf(TEXT("Simulated Proxy Wall Locaton = ")))

			
		// Executed Client and Server
		if (Player->Role == ROLE_Authority)
			HangOnLedge();
	

	}

}




void UFPSPlayerJump::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSPlayerJump, IsCharacterJump);
	DOREPLIFETIME(UFPSPlayerJump, HitLedgeUp);
	DOREPLIFETIME(UFPSPlayerJump, WallLocation);
	DOREPLIFETIME(UFPSPlayerJump, WallNormal);
	DOREPLIFETIME(UFPSPlayerJump, IsHeigthLedge);
	DOREPLIFETIME(UFPSPlayerJump, JumpData);


}



void UFPSPlayerJump::JumpUp()
{
	if (Player->bHangOnLedge)
	{
		SR_Jump();
		
		if (Player->Role == ROLE_AutonomousProxy)
		{
			SetJumpMode(true);
			ClimbOnLedge();
		}


	}
	else
	{

		if (!Player->IsExitLedge)
			SR_ChackLedgeHeight();


		if (Player->Role < ROLE_Authority)
		{
			SetJumpMode(true);
			Player->PlayAnimMontage(JumpAnim, 1.f, "JumpUp");


		}

		SR_Jump();


	}
}

void UFPSPlayerJump::SR_Jump_Implementation()
{

	if (Player->bHangOnLedge)
	{
		SetJumpMode(true);
		ClimbOnLedge();
		MC_ClimbOnLedge();
	}
	else
	{
		SetJumpMode(true);
		Player->PlayAnimMontage(JumpAnim, 1.f, "JumpUp");

		MC_Jump();
	}
	
}

bool UFPSPlayerJump::SR_Jump_Validate()
{
	return true;
}



void UFPSPlayerJump::MC_Jump_Implementation()
{
	if (!Player->IsLocallyControlled())
	{
		//SetJumpMode(true);
		Player->PlayAnimMontage(JumpAnim, 1.f, "JumpUp");
		LOG_S(FString("MC_Jump"));
	}
}

void UFPSPlayerJump::JumpDown()
{

	LOG_S(FString("Jump And JumpDown"));

	if (Player->Role == ROLE_AutonomousProxy)
	{
		SetJumpMode(false);
		Player->PlayAnimMontage(JumpAnim, 1.5f, "JumpDown");

		IsHeigthLedge = false;
		Player->bHangOnLedge = false;
		Player->IsExitLedge = false;
		NextUUID = 0;
	}

	SR_JumpDown();
}

void UFPSPlayerJump::SR_JumpDown_Implementation()
{
	Player->PlayAnimMontage(JumpAnim, 1.5f, "JumpDown");
	SetJumpMode(false);

	IsHeigthLedge = false;
	Player->bHangOnLedge = false;
	Player->IsExitLedge = false;
	NextUUID = 0;

	MC_JumpDown();
}

bool UFPSPlayerJump::SR_JumpDown_Validate()
{
	return true;
}


void UFPSPlayerJump::MC_JumpDown_Implementation()
{
	if (Player && !Player->IsLocallyControlled())
	{
		// Called Only SimulatedProxy
		SetJumpMode(false);
		Player->PlayAnimMontage(JumpAnim, 1.f, "JumpDown");
		LOG_S(FString("MC_Jump"));
	}
}

void UFPSPlayerJump::SetJumpMode(bool val)
{

	Player->bJump = val;
	if (Player->bJump)
	{
		Player->GetCharacterMovement()->MaxWalkSpeed = 200.f/*Player->GetDefaultCrouchSpeed()*/;
	}
	else
	{
		Player->GetCharacterMovement()->MaxWalkSpeed = Player->GetDefalutMaxSpeed();
	}
}



void UFPSPlayerJump::HangOnLedge()
{

	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	FLatentActionInfo HangOnInfo;
	HangOnInfo.CallbackTarget = this;
	HangOnInfo.ExecutionFunction = FName("HangFinished");
	HangOnInfo.Linkage = 0;
	HangOnInfo.UUID = GetNextUUID();

	UKismetSystemLibrary::MoveComponentTo(
		Player->GetCapsuleComponent(),
		JumpData.TargetLocation,
		JumpData.TargetRotation/*Player->GetActorRotation()*/,
		false,
		false,
		0.5,
		false,
		EMoveComponentAction::Move,
		HangOnInfo
	);

	Player->bHangOnLedge = true;
	IsHeigthLedge = false;
	LOG_I(IsCharacterJump);

	if (Player->Role == ROLE_Authority)
		MC_HangOnLedge();

	LOG_S(FString("Jump and Hand On"))
}

void UFPSPlayerJump::HangFinished()
{


	Player->PlayAnimMontage(ClimbAnim, 1.f, "ClimbPose");
	Player->GetCharacterMovement()->StopMovementImmediately();
	LOG_S(FString::Printf(TEXT("Jump And MoveToTargetFinished Time Secnde = %f"), UGameplayStatics::GetTimeSeconds(this)));
	IsHeigthLedge = false;



	//TimerDel.BindUFunction(this, FName("SetJumpState"), EJumpState::None);
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_Climb, TimerDel, 1.f, false);
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_Climb, this, &UFPSPlayerJump::FixToLedge, 0.5, false);
	//UGameplayStatics::SetGlobalTimeDilation(Player, 0.1);
}

void UFPSPlayerJump::MC_HangOnLedge_Implementation()
{
	//if (Player->Role == ROLE_SimulatedProxy)
	if (Player->Role < ROLE_Authority)
	{
		HangOnLedge();
	}
}

void UFPSPlayerJump::ClimbOnLedge()
{

	
	Player->SetPermissionMoving(false);


	Player->PlayAnimMontage(ClimbAnim, 1.f, "ClimbOn");
	int32 sectionIndex = ClimbAnim->GetSectionIndex("ClimbOn");
	float Timedelation = ClimbAnim->GetSectionLength(sectionIndex);
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	//UGameplayStatics::SetGlobalTimeDilation(Player, 0.1);


	if (IsPlayerChangePosition())
		ForwardTracer();

	setJumpData(EJumpState::ClimbOn);

	FLatentActionInfo ClimbOnInfo;
	ClimbOnInfo.CallbackTarget = this;
	ClimbOnInfo.Linkage = 0;
	ClimbOnInfo.UUID = GetNextUUID();



	UKismetSystemLibrary::MoveComponentTo(
		Player->GetCapsuleComponent(),
		JumpData.TargetLocation,
		JumpData.TargetRotation,
		false,
		false,
		0.7f,
		false,
		EMoveComponentAction::Move,
		ClimbOnInfo
	);


	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Climb, this, &UFPSPlayerJump::ClimbFinished, Timedelation, false);
}

void UFPSPlayerJump::ClimbFinished()
{

	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Player->StopAnimMontage(ClimbAnim);
	SetJumpMode(false);
	Player->bHangOnLedge = false;

	
	Player->SetPermissionMoving(true);

}




void UFPSPlayerJump::SR_ChackLedgeHeight_Implementation()
{

	LedgeLength = 0;

	ForwardTracer();

	if (LedgeLength >= 80)
	{
		setJumpData(EJumpState::HangOn);
		IsHeigthLedge = true;
		LOG_F(LedgeLength);
	}
	else
	{
		IsHeigthLedge = false;
		LOG_F(LedgeLength);
	}
}

bool UFPSPlayerJump::SR_ChackLedgeHeight_Validate()
{
	return true;
}





void UFPSPlayerJump::MC_ClimbOnLedge_Implementation()
{
	if (Player->Role == ROLE_SimulatedProxy)
	{

		/*if (Player->Role == ROLE_Authority)
			LOG_S(FString::Printf(TEXT("Server WallLocation MC = %s JumpDataLocaton = %s"), *WallLocation.ToString(), *JumpData.TargetLocation.ToString()));
		if (Player->Role == ROLE_AutonomousProxy)
			LOG_S(FString::Printf(TEXT("Clinet WallLocation MC = %s JumpDataLocaton = %s"), *WallLocation.ToString(), *JumpData.TargetLocation.ToString()));
		if (Player->Role == ROLE_SimulatedProxy)
			LOG_S(FString::Printf(TEXT("SimulatedProxy WallLocation MC = %s JumpDataLocaton = %s"), *WallLocation.ToString(), *JumpData.TargetLocation.ToString()));
*/
		ClimbOnLedge();
	}
}

void UFPSPlayerJump::ForwardTracer()
{
	FVector StartPoint = Player->GetActorLocation();
	FVector EndPoint = (Player->GetActorForwardVector() * 300.f) + StartPoint;


	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player);
	QueryParams.bTraceComplex = true;
	//QueryParams.bReturnPhysicalMaterial = true;


	FHitResult Hit;
	bool LTSC_1 = GetWorld()->LineTraceSingleByChannel(
		OUT Hit,
		StartPoint,
		EndPoint,
		COLLISION_Jump,
		QueryParams
	);

	WallLocation = Hit.Location;
	WallNormal = Hit.ImpactNormal;
	//LOG_S(WallNormal.ToString());


	if (DrawDebugJumpLine)
	{
		DrawDebugLine(
			GetWorld(),
			StartPoint,
			EndPoint,
			FColor(255, 0, 0),
			false,
			10.f,
			0.f,
			5.f
		);

		DrawDebugSphere(GetWorld(), WallLocation, 12, 6, FColor::Red, true, 10.f);
	}

	bool LTSC_2 = false;


	if (LTSC_1)
	{
		FVector StartPoint = Hit.ImpactPoint + (Hit.ImpactNormal * -InnerTraceDeep) + FVector(0, 0, MaxJumpLength);
		FVector EndPoint = Hit.ImpactPoint + (Hit.ImpactNormal * -InnerTraceDeep);

		LTSC_2 = GetWorld()->LineTraceSingleByChannel(
			OUT HitLedgeUp,
			StartPoint,
			EndPoint,
			COLLISION_Jump,
			QueryParams
		);


		if (LTSC_2 && HitLedgeUp.Distance != 0)
			LedgeLength = MaxJumpLength - HitLedgeUp.Distance;


		if (DrawDebugJumpLine)
		{
			DrawDebugLine(
				GetWorld(),
				StartPoint,
				EndPoint,
				FColor(0, 255, 0),
				false,
				10.f,
				0.f,
				5.f
			);
		}
	}

}

FVector UFPSPlayerJump::CalculateHangOnLocation()
{
	FVector MoveLocation = HitLedgeUp.Location;
	MoveLocation.Z -= Player->GetPlayerHeight();

	if (WallNormal.X > 0 || WallNormal.Y > 0)
	{
		if (WallNormal.X > 0)
		{
			MoveLocation.X = WallLocation.X + Player->GetDistanceToLedge();
		}
		else
		{
			MoveLocation.Y = WallLocation.Y + Player->GetDistanceToLedge();
		}

	}
	else
	{

		if (WallNormal.X < 0)
		{
			MoveLocation.X = WallLocation.X - Player->GetDistanceToLedge();
		}
		else
		{
			MoveLocation.Y = WallLocation.Y - Player->GetDistanceToLedge();
		}

	}



	return MoveLocation;
}

FRotator UFPSPlayerJump::CalculateHangOnRotation()
{
	float Roll = Player->GetActorRotation().Roll;
	float Pitch = Player->GetActorRotation().Pitch;
	float Yaw = WallNormal.Rotation().Yaw - 180;
	FRotator MoveRotation = FRotator(Pitch, Yaw, Roll);

	return MoveRotation;
}


bool UFPSPlayerJump::IsPlayerChangePosition()
{
	return int(WallLocation.X) != int(Player->GetActorLocation().X) && int(WallLocation.Y) != int(Player->GetActorLocation().Y) ? true : false;
}

void UFPSPlayerJump::setJumpData(EJumpState jumpstate)
{
	switch (jumpstate)
	{
	case EJumpState::HangOn:
		JumpData.TargetLocation = CalculateHangOnLocation();
		JumpData.TargetRotation = CalculateHangOnRotation();
		LOG_S(Player->GetName());
		break;
	case EJumpState::ClimbOn:
		JumpData.TargetLocation.X = WallLocation.X;
		JumpData.TargetLocation.Y = WallLocation.Y;
		JumpData.TargetLocation.Z = HitLedgeUp.Location.Z + Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		JumpData.TargetLocation -= (Player->GetActorForwardVector() * 10);
		break;
	default:
		break;
	}
}


