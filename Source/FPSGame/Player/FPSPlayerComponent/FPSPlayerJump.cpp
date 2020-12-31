// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerJump.h"
#include "../FPSMannequin.h"
#include "../../DebugTool/DebugLog.h"

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
	if (IsCharacterJump)
	{
		/*if (Player->Role == ROLE_Authority)
			LOG_S(FString::Printf(TEXT("Server ActorLocation = %s"), *Player->GetActorLocation().ToString()));
		if (Player->Role == ROLE_AutonomousProxy)
			LOG_S(FString::Printf(TEXT("Clinet ActorLocation = %s"), *Player->GetActorLocation().ToString()));
		if (Player->Role == ROLE_SimulatedProxy)
			LOG_S(FString::Printf(TEXT("Simulated Proxy Wall Locaton = ")))*/
		
		if (IsHeigthLedge)
		{
			/*if (Player->Role == ROLE_Authority)
				LOG_S(FString::Printf(TEXT("Server WallLocation ClimbOnLedge = %s JumpDataLocaton = %s"), *WallLocation.ToString(), *JumpData.TargetLocation.ToString()));
			if (Player->Role == ROLE_AutonomousProxy)
				LOG_S(FString::Printf(TEXT("Clinet WallLocation ClimbOnLedge = %s JumpDataLocaton = %s"), *WallLocation.ToString(), *JumpData.TargetLocation.ToString()));
			if (Player->Role == ROLE_SimulatedProxy)
				if (Player->Role == ROLE_SimulatedProxy)
					LOG_S(FString::Printf(TEXT("SimulatedProxy WallLocation MC_HangOn = %s JumpDataLocaton = %s"), *WallLocation.ToString(), *JumpData.TargetLocation.ToString()));
*/
			// Executed Client and Server
			HangOnLedge();
		}
			
	}

}




void UFPSPlayerJump::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSPlayerJump, IsCharacterJump);
	//DOREPLIFETIME(UFPSPlayerJump, SuccessLineTrace);
	//DOREPLIFETIME(UFPSPlayerJump, LedgeLength);
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

		SetJumpMode(true);
		SR_ClimbOnLedge();
		if (Player->Role == ROLE_AutonomousProxy)
			ClimbOnLedge();

	}
	else
	{

		if (!Player->IsExitLedge/* && Player->Role < ROLE_Authority*/)
			SR_CalculateJumpState();


		if (Player->Role < ROLE_Authority)
		{
			SetJumpMode(true);
			Player->PlayAnimMontage(JumpAnim, 1.f, "JumpUp");
			//JumpState = CalculateJumpState();

		}

		SR_Jump();
	}
}

void UFPSPlayerJump::SR_Jump_Implementation()
{
	SetJumpMode(true);
	Player->PlayAnimMontage(JumpAnim, 1.f, "JumpUp");

	MC_Jump();
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

	if (Player->Role < ROLE_Authority)
	{
		SetJumpMode(false);
		Player->PlayAnimMontage(JumpAnim, 1.5f, "JumpDown");
	}
	


	IsHeigthLedge = false;
	Player->bHangOnLedge = false;
	Player->IsExitLedge = false;
	NextUUID = 0;

	SR_JumpDown();
}

void UFPSPlayerJump::SR_JumpDown_Implementation()
{
	Player->PlayAnimMontage(JumpAnim, 1.5f, "JumpDown");
	SetJumpMode(false);

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
	if (Player->Role < ROLE_Authority)
		SR_JumpMode(val);

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

void UFPSPlayerJump::SR_JumpMode_Implementation(bool val)
{
	SetJumpMode(val);
}

bool UFPSPlayerJump::SR_JumpMode_Validate(bool val)
{
	if (Player->GetCharacterMovement()->MaxWalkSpeed < 200 || Player->GetCharacterMovement()->MaxWalkSpeed > Player->GetDefalutMaxSpeed())
	{
		return false;
	}
	else
	{
		return true;
	}
}




void UFPSPlayerJump::HangOnLedge()
{


	if (Player->Role == ROLE_Authority)
		MC_HangOnLedge();



	FLatentActionInfo HangOnInfo;
	HangOnInfo.CallbackTarget = this;
	HangOnInfo.ExecutionFunction = FName("MoveToTargetFinished");
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

	//UGameplayStatics::SetGlobalTimeDilation(Player, 0.1);

	LOG_S(FString("Jump and Hand On"))
}

void UFPSPlayerJump::MoveToTargetFinished()
{

	/*if (Player->Role == ROLE_Authority)
		LOG_S(FString::Printf(TEXT("Server Wall Locaton = %s"), *WallLocation.ToString()));
	if (Player->Role == ROLE_AutonomousProxy)
		LOG_S(FString::Printf(TEXT("Clinet Wall Locaton = %s"), *WallLocation.ToString()));
	if (Player->Role == ROLE_SimulatedProxy)
		LOG_S(FString::Printf(TEXT("Simulated Proxy Wall Locaton = ")))*/

	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

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
	if (Player->Role == ROLE_SimulatedProxy)
	{
		HangOnLedge();
	}
}

void UFPSPlayerJump::ClimbOnLedge()
{

	if (Player->IsLocallyControlled())
	{
		Player->SetPermissionMoving(false);
	}

	Player->PlayAnimMontage(ClimbAnim, 1.f, "ClimbOn");
	int32 sectionIndex = ClimbAnim->GetSectionIndex("ClimbOn");
	float Timedelation = ClimbAnim->GetSectionLength(sectionIndex);
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	//UGameplayStatics::SetGlobalTimeDilation(Player, 0.1);


	if (int(WallLocation.X) != int(Player->GetActorLocation().X) && int(WallLocation.Y) != int(Player->GetActorLocation().Y))
	{
		bool LineTrace = ForwardTracer();
		if (!LineTrace)
			return;

	}

	JumpData.TargetLocation.X = WallLocation.X;
	JumpData.TargetLocation.Y = WallLocation.Y;
	JumpData.TargetLocation.Z = HitLedgeUp.Location.Z + Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	JumpData.TargetLocation -= (Player->GetActorForwardVector() * 10);

	FLatentActionInfo ClimbOnInfo;
	ClimbOnInfo.CallbackTarget = this;
	ClimbOnInfo.Linkage = 0;
	ClimbOnInfo.UUID = GetNextUUID();



	UKismetSystemLibrary::MoveComponentTo(
		Player->GetCapsuleComponent(),
		JumpData.TargetLocation,
		JumpData.TargetRotation/*Player->GetActorRotation()*/,
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

	if (Player->IsLocallyControlled())
	{
		Player->SetPermissionMoving(true);
	}
}



void UFPSPlayerJump::SR_CalculateJumpState_Implementation()
{
	LedgeLength = 0;

	SuccessLineTrace = ForwardTracer();


	//LOG_S(FString::Printf(TEXT("Jump LTSC_1 = %i, LTSC_2 = %i"), LTSC_1, LTSC_2));
	//LOG_F(LedgeLength);

	FVector SocketLocation = Player->GetMesh()->GetSocketLocation(Socket);
	float HipToLedge = SocketLocation.Z - HitLedgeUp.Location.Z;
	//LOG_F(HipToLedge);

	if (SuccessLineTrace && LedgeLength >= 80)
	{
		CalculateJumpData();
		IsHeigthLedge = true;
	}
	else
	{
		IsHeigthLedge = false;
	}
}

bool UFPSPlayerJump::SR_CalculateJumpState_Validate()
{
	return true;
}



void UFPSPlayerJump::SR_ClimbOnLedge_Implementation()
{

	/*if (int(WallLocation.X) != int(Player->GetActorLocation().X) && int(WallLocation.Y) != int(Player->GetActorLocation().Y))
	{
		bool LineTrace = ForwardTracer();
		if (!LineTrace)
			return;
		
	}

	JumpData.TargetLocation.X = WallLocation.X;
	JumpData.TargetLocation.Y = WallLocation.Y;
	JumpData.TargetLocation.Z = HitLedgeUp.Location.Z + Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	JumpData.TargetLocation -= (Player->GetActorForwardVector() * 10);*/


	ClimbOnLedge();
	MC_ClimbOnLedge();
}

bool UFPSPlayerJump::SR_ClimbOnLedge_Validate()
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

bool UFPSPlayerJump::ForwardTracer()
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


		if (HitLedgeUp.Distance != 0)
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

	return LTSC_2;
}

FVector UFPSPlayerJump::CalculateHangOnLocation()
{
	FVector MoveLocation = HitLedgeUp.Location;
	MoveLocation.Z -= (Player->GetMesh()->GetSocketLocation(Socket).Z + 45); //(Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	if (WallNormal.X > 0 || WallNormal.Y > 0)
	{
		if (WallNormal.X > 0)
		{
			MoveLocation.X = WallLocation.X + 35;
		}
		else
		{
			MoveLocation.Y = WallLocation.Y + 35;
		}
		
	}
	else
	{
	
		if (WallNormal.X < 0)
		{
			MoveLocation.X = WallLocation.X - 35;
		} 
		else
		{
			MoveLocation.Y = WallLocation.Y - 35;
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




void UFPSPlayerJump::CalculateJumpData()
{
	
	JumpData.TargetLocation = CalculateHangOnLocation();
	JumpData.TargetRotation = CalculateHangOnRotation();

	//return JumpData;
}

// =======================================



//EJumpState UFPSPlayerJump::CalculateJumpState()
//{
//	/*if (Player->Role < ROLE_Authority)
//		SR_CalculateJumpState();*/
//
//	LedgeLength = 0;
//
//	SuccessLineTrace = ForwardTracer();
//
//
//	//LOG_S(FString::Printf(TEXT("Jump LTSC_1 = %i, LTSC_2 = %i"), LTSC_1, LTSC_2));
//	//LOG_F(LedgeLength);
//
//	FVector SocketLocation = Player->GetMesh()->GetSocketLocation(Socket);
//	float HipToLedge = SocketLocation.Z - HitLedge.Location.Z;
//	//LOG_F(HipToLedge);
//
//	if (SuccessLineTrace && LedgeLength > 0)
//	{
//
//		if (LedgeLength <= 80)
//		{
//			return EJumpState::Jump;
//		}
//		else
//		{
//			return EJumpState::HangOn;
//		}
//
//	}
//	else
//	{
//		return EJumpState::None;
//	}
//
//
//}

