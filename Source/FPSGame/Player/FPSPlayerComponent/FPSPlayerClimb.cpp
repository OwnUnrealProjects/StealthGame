// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerClimb.h"
#include "../FPSMannequin.h"
#include "../../DebugTool/DebugLog.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimMontage.h"


// Sets default values for this component's properties
UFPSPlayerClimb::UFPSPlayerClimb()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UFPSPlayerClimb::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AFPSMannequin>(GetOwner());

	if (DrawDebugTraceCapsule)
	{
		DebugTracer = EDrawDebugTrace::ForOneFrame;
	}
	else
	{
		DebugTracer = EDrawDebugTrace::None;
	}
	
}


// Called every frame
void UFPSPlayerClimb::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	PlayerInputDirection = Player->InRight * Player->GetPermissionMoving();
	if (Player->bHangOnLedge)
	{
		if (PlayerInputDirection < 0)
		{
			Player->bJump = false;
			MoveLedgeLeft(DeltaTime);
		}
		if (PlayerInputDirection > 0)
		{
			Player->bJump = false;
			MoveLedgeRight(DeltaTime);
		}

		if (Player->InForward < 0)
			ExitLedge();
	}
}


void UFPSPlayerClimb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFPSPlayerClimb, bMoveLeft);
	DOREPLIFETIME(UFPSPlayerClimb, bMoveRight);
	DOREPLIFETIME(UFPSPlayerClimb, CurrentV);
	DOREPLIFETIME(UFPSPlayerClimb, TargetV);
	DOREPLIFETIME(UFPSPlayerClimb, MoveLocation);

}

bool UFPSPlayerClimb::LedgeLeftTracer()
{
	const FVector Start = Player->GetLeftArrowComponent()->GetComponentLocation();
	const FVector End = Player->GetLeftArrowComponent()->GetComponentLocation();

	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	bool CanMoveLeft = UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		Start,
		End,
		20,
		60,
		TraceTypeQuery4,
		false,
		IgnoreActors,
		DebugTracer,
		Hit,
		true
	);

	return CanMoveLeft;
	LOG_S(FString::Printf(TEXT("ClimbComponent LeftTracer bool = %i"), CanMoveLeft));
}

bool UFPSPlayerClimb::LedgeRightTracer()
{
	const FVector Start = Player->GetRightArrowComponent()->GetComponentLocation();
	const FVector End = Player->GetRightArrowComponent()->GetComponentLocation();

	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	bool CanMoveRight = UKismetSystemLibrary::CapsuleTraceSingle(
		this,
		Start,
		End,
		20,
		60,
		TraceTypeQuery4,
		false,
		IgnoreActors,
		DebugTracer,
		Hit,
		true
	);
	
	return CanMoveRight;
	LOG_S(FString::Printf(TEXT("ClimbComponent RightTracer bool = %i"), CanMoveRight));
}

bool UFPSPlayerClimb::CornerLeftTracer()
{
	Player->GetCharacterMovement()->StopMovementImmediately();

	FVector Start = Player->GetLeftArrowComponent()->GetComponentLocation();
	Start.Z += 60.f;
	const FVector End = Start + (Player->GetLeftArrowComponent()->GetForwardVector() * 70.f);

	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	bool CanTurnLeft = UKismetSystemLibrary::SphereTraceSingle(
		this,
		Start,
		End,
		20.f,
		TraceTypeQuery4,
		false,
		IgnoreActors,
		DebugTracer,
		Hit,
		true
	);

	return CanTurnLeft;
}

bool UFPSPlayerClimb::ConerRightTracer()
{
	Player->GetCharacterMovement()->StopMovementImmediately();

	FVector Start = Player->GetRightArrowComponent()->GetComponentLocation();
	Start.Z += 60.f;
	const FVector End = Start + (Player->GetRightArrowComponent()->GetForwardVector() * 70.f);

	FHitResult Hit;
	TArray<AActor*> IgnoreActors;
	bool CanTurnRight = UKismetSystemLibrary::SphereTraceSingle(
		this,
		Start,
		End,
		20.f,
		TraceTypeQuery4,
		false,
		IgnoreActors,
		DebugTracer,
		Hit,
		true
	);

	return CanTurnRight;
}

void UFPSPlayerClimb::MoveLedgeLeft(float DeltaT)
{
	if (LedgeLeftTracer())
	{
		CurrentV = Player->GetActorLocation();
		TargetV = CurrentV + (Player->GetActorRightVector() * -20);
		MoveLocation = FMath::VInterpTo(CurrentV, TargetV, DeltaT, 5.f);
		Player->SetActorLocation(MoveLocation);
	
		bMoveLeft = true;
		bMoveRight = false;


		ClimbSate = EClimbState::ClimbLine;
		LOG_S(FString("LedgeTracer"))

	}
	else
	{
		PlayerInputDirection = 0;
	}
	/*else if (!CornerLeftTracer())
	{
		Player->PlayAnimMontage(ClimbCornerAnim, 1.f, "CornerLeft");
		ClimbSate = EClimbState::ClimbCorner;
		LOG_S(FString("CornerTracer"))
	}*/
}

void UFPSPlayerClimb::MoveLedgeRight(float DeltaT)
{
	if (LedgeRightTracer())
	{
		CurrentV = Player->GetActorLocation();
		TargetV = CurrentV + (Player->GetActorRightVector() * 20);
		MoveLocation = FMath::VInterpTo(CurrentV, TargetV, DeltaT, 5.f);
		Player->SetActorLocation(MoveLocation);

		bMoveLeft = false;
		bMoveRight = true;

		
		ClimbSate = EClimbState::ClimbLine;

	}
	else
	{
		PlayerInputDirection = 0;
	}
	/*else if (!ConerRightTracer())
	{
		ClimbSate = EClimbState::ClimbCorner;
		Player->PlayAnimMontage(ClimbCornerAnim, 1.f, "CornerRight");
	}*/
}

void UFPSPlayerClimb::ExitLedge()
{
	Player->bHangOnLedge = false;
	Player->IsExitLedge = true;
	Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	Player->JumpEvent();
}

