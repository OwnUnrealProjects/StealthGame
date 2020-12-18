// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerJump.h"
#include "../FPSMannequin.h"
#include "../../DebugTool/DebugLog.h"

#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

#define OUT

// Sets default values for this component's properties
UFPSPlayerJump::UFPSPlayerJump()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
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
		LOG_I(IsCharacterJump);
		switch (JumpState)
		{
		case EJumpState::None:
			//LOG_S(FString("None = JumpState"))
				break;
		case EJumpState::Jump:
			JumpOnLedge();
			//LOG_S(FString("Jump = JumpState"))
				break;
		case EJumpState::Climbing:
			HangOnLedge();
			//LOG_S(FString("Hang = JumpState"));
			break;
		default:
			break;
		}
	}
	//LOG_I(IsCharacterJump);
}




void UFPSPlayerJump::JumpUp()
{
	if (IsHangOn)
	{
		ClimbOnLedge();
	}
	else
	{
		SetJumpMode(true);
		Player->PlayAnimMontage(JumpAnim, 1.f, "JumpUp");
		//LOG_S(FString("Jump Up"));

		JumpState = CalculateJumpState();
	}
}


void UFPSPlayerJump::JumpDown()
{
	LOG_S(FString("Jump And JumpDown"));
	SetJumpMode(false);
	Player->PlayAnimMontage(JumpAnim, 1.5f, "JumpDown");

	JumpState = EJumpState::None;
	IsHangOn = false;
}



void UFPSPlayerJump::SetJumpMode(bool val)
{
	if (Player->Role < ROLE_Authority)
		SR_Jump(val);

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

void UFPSPlayerJump::SR_Jump_Implementation(bool val)
{
	SetJumpMode(val);
	if (val)
	{
		Player->PlayAnimMontage(JumpAnim, 1.f, "JumpUp");
	}
	else
	{
		Player->PlayAnimMontage(JumpAnim, 1.5f, "JumpDown");
	}
	
}

bool UFPSPlayerJump::SR_Jump_Validate(bool val)
{
	return true;
}


void UFPSPlayerJump::JumpOnLedge()
{
	//bJumpOnce = true;

	FVector MoveLocation = HitLedge.ImpactPoint;
	MoveLocation.Z += Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(); // It's same CapsulHalfHeight



	FLatentActionInfo JumpOnInfo;
	JumpOnInfo.CallbackTarget = this;
	JumpOnInfo.Linkage = 0;
	JumpOnInfo.UUID = GetNextUUID();

	UKismetSystemLibrary::MoveComponentTo(
		Player->GetCapsuleComponent(),
		MoveLocation,
		Player->GetActorRotation(),
		false,
		false,
		0.3,
		false,
		EMoveComponentAction::Move,
		JumpOnInfo
	);

	//bJumpOnce = false;
	LOG_S(FString::Printf(TEXT("Jump And Move Time Secnde = %f"), UGameplayStatics::GetTimeSeconds(this)));



}

void UFPSPlayerJump::HangOnLedge()
{
	
	FRotator TargetRotation = CalculateTargetRotation();

	FVector TargetLocation = CalculateTargetLocation();

	LOG_S(HitLedge.Location.ToString());
	LOG_S(WallNormal.ToString());
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	FLatentActionInfo ClimbOnInfo;
	ClimbOnInfo.CallbackTarget = this;
	ClimbOnInfo.ExecutionFunction = FName("MoveToTargetFinished");
	ClimbOnInfo.Linkage = 0;
	ClimbOnInfo.UUID = GetNextUUID();

	UKismetSystemLibrary::MoveComponentTo(
		Player->GetCapsuleComponent(),
		TargetLocation,
		TargetRotation/*Player->GetActorRotation()*/,
		false,
		false,
		0.5,
		false,
		EMoveComponentAction::Move,
		ClimbOnInfo
	);

	IsHangOn = true;
}

void UFPSPlayerJump::ClimbOnLedge()
{
	Player->PlayAnimMontage(ClimbAnim, 1.f, "ClimbOn");
	int32 sectionIndex = ClimbAnim->GetSectionIndex("ClimbOn");
	float Timedelation = ClimbAnim->GetSectionLength(sectionIndex);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_Climb, this, &UFPSPlayerJump::ClimbFinished, Timedelation, false);
	//Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void UFPSPlayerJump::ClimbFinished()
{
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void UFPSPlayerJump::MoveToTargetFinished()
{
	
	
	Player->PlayAnimMontage(ClimbAnim, 1.f, "ClimbPose");
	Player->GetCharacterMovement()->StopMovementImmediately();
	LOG_S(FString::Printf(TEXT("Jump And MoveToTargetFinished Time Secnde = %f"), UGameplayStatics::GetTimeSeconds(this)));
	//TimerDel.BindUFunction(this, FName("SetJumpState"), EJumpState::None);
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_Climb, TimerDel, 1.f, false);
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle_Climb, this, &UFPSPlayerJump::FixToLedge, 0.5, false);

	//UGameplayStatics::SetGlobalTimeDilation(Player, 0.1);
}

EJumpState UFPSPlayerJump::CalculateJumpState()
{
	LedgeLength = 0;

	FVector StartPoint = Player->GetActorLocation();
	FVector EndPoint = (Player->GetActorForwardVector() * 200.f) + StartPoint;


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
	}

	bool LTSC_2 = false;


	if (LTSC_1)
	{
		FVector StartPoint = Hit.ImpactPoint + (Hit.ImpactNormal * -50) + FVector(0, 0, MaxJumpLength);
		FVector EndPoint = Hit.ImpactPoint + (Hit.ImpactNormal * -50);

		LTSC_2 = GetWorld()->LineTraceSingleByChannel(
			OUT HitLedge,
			StartPoint,
			EndPoint,
			COLLISION_Jump,
			QueryParams
		);


		if(HitLedge.Distance != 0)
			LedgeLength = MaxJumpLength - HitLedge.Distance;


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

	//LOG_S(FString::Printf(TEXT("Jump LTSC_1 = %i, LTSC_2 = %i"), LTSC_1, LTSC_2));
	//LOG_F(LedgeLength);

	FVector SocketLocation = Player->GetMesh()->GetSocketLocation(Socket);
	float HipToLedge = SocketLocation.Z - HitLedge.Location.Z;
	//LOG_F(HipToLedge);

	if (LTSC_2 && LedgeLength > 0)
	{

		if (LedgeLength <= 80)
		{
			return EJumpState::Jump;
		}
		else
		{
			return EJumpState::Climbing;
		}

	}
	else
	{
		return EJumpState::None;
	}

}

FVector UFPSPlayerJump::CalculateTargetLocation()
{
	FVector MoveLocation = HitLedge.Location;
	MoveLocation.Z -= Player->GetMesh()->GetSocketLocation(Socket).Z; //(Player->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	if (WallNormal.X > 0 || WallNormal.Y > 0)
	{
		MoveLocation.X = Player->GetMesh()->GetSocketLocation(Socket).X + 10;
		MoveLocation.Y = Player->GetMesh()->GetSocketLocation(Socket).Y + 10;
	}
	else
	{
		MoveLocation.X = Player->GetMesh()->GetSocketLocation(Socket).X - 10;
		MoveLocation.Y = Player->GetMesh()->GetSocketLocation(Socket).Y - 10;
	}
	
	return MoveLocation;
}

FRotator UFPSPlayerJump::CalculateTargetRotation()
{
	float Roll = Player->GetActorRotation().Roll;
	float Pitch = Player->GetActorRotation().Pitch;
	float Yaw = WallNormal.Rotation().Yaw - 180;
	FRotator MoveRotation = FRotator(Pitch, Yaw, Roll);

	return MoveRotation;
}
