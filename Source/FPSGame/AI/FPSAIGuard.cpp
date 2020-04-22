// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"

#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Net/UnrealNetwork.h"  // contains DOREPLIFETIME macro
#include "Engine.h"

#include "FPSGameMode.h"
#include "../Public/FPSCharacter.h"


// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// This need not attachment because it's not SceneComponent and has not location rotation and so on
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnPawnHearing);

	GuardState = EAIState::Idle;


}

void AFPSAIGuard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFPSAIGuard, GuardState);
}


void AFPSAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();
	
	OriginalRotation = GetActorRotation();
	GuardController = GetController();

	if (bPatrol)
	{
		MoveNextPatrolPoint();
		UE_LOG(LogTemp, Warning, TEXT("Begin PatrolPoint"));
	}

}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Patrol Goal Check
	if (CurrentPatrolPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tick PatrolPoint_1"));
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();

		// Check if we are within 50 units of our goal, if so pick a new patrol point
		if (DistanceToGoal < 100.f && bCanTick)
		{
			GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
			GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::MoveNextPatrolPoint, 3.0f);
			bCanTick = false;
			UE_LOG(LogTemp, Warning, TEXT("Tick PatrolPoint"));
		}
	}

	DrawDebugString(GetWorld(), FVector(0, 0, 100.f), GetEnumText(Role), this, FColor::Green, DeltaTime);

}


FString AFPSAIGuard::GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

FString AFPSAIGuard::GetStateText(EAIState state)
{
	switch (state)
	{
	case EAIState::Idle:
		return "Idle";
	case EAIState::Suspisious:
		return "Suspicious";
	case EAIState::Alert:
		return "Alert";
	default:
		return "Error";
	}
}

void AFPSAIGuard::OnPawnSeen(APawn * SeenPawn)
{
	auto Gualty = Cast<AFPSCharacter>(SeenPawn);
	if (!Gualty)
		return;
	//UE_LOG(LogTemp, Warning, TEXT("SeenPawn"));
	
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 40.f, 10.f, FColor::Yellow, false, 10.f);

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM && Gualty->bGuardSeen)
	{

		Gualty->bGuardSeen = false;
		if (Gualty->bIsCarryingObjective)
			Gualty->bReturnObjective = true;
		GM->MissionFaild(SeenPawn);
		
	}

	
	FVector Direction = SeenPawn->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.f;
	NewLookAt.Roll = 0.f;

	SetActorRotation(NewLookAt);

	SetAIState(EAIState::Alert);

	// Stop Movement if Patrolling
	if (GuardController)
		GuardController->StopMovement();

}

void AFPSAIGuard::OnPawnHearing(APawn* NoiseInstigator, const FVector & Location, float Volume)
{
	if (!NoiseInstigator && GuardState == EAIState::Alert)
		return;


	UE_LOG(LogTemp, Warning, TEXT("Hearing Perception"));
	DrawDebugSphere(GetWorld(), Location, 40.f, 10.f, FColor::Green, false, 10.f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.f;
	NewLookAt.Roll = 0.f;

	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

	UE_LOG(LogTemp, Error, TEXT("Hearing Perception_2"));

	SetAIState(EAIState::Suspisious);

	// Stop Movement if Patrolling
	if (GuardController)
		GuardController->StopMovement();


	DrawDebugString(GetWorld(), FVector(400), GetEnumText(Role), this, FColor::Green, 5.f);

}

void AFPSAIGuard::ResetOrientation()
{
	UE_LOG(LogTemp, Warning, TEXT("ResetOrinetaion"));
	if (GuardState == EAIState::Alert)
		return;

	SetActorRotation(OriginalRotation);
	SetAIState(EAIState::Idle);

	// Stopped Investigating if we are a patrolling pawn, pick a new patrol point to move to
	if (bPatrol)
		MoveNextPatrolPoint();
}

void AFPSAIGuard::SetAIState(EAIState State)
{
	if (GuardState == State)
		return;

	GuardState = State;

	OnStateChanged(GuardState);
	
	UE_LOG(LogTemp, Error, TEXT("Both Server and Client"));
	

	/// Simulated Proxy don't executed
	/*if (Role == ROLE_SimulatedProxy)
	{
		DrawDebugString(GetWorld(), FVector(200), GetStateText(GuardState), this, FColor::Green, 10.f);
		UE_LOG(LogTemp, Warning, TEXT("Simulated Proxy = %s"), *GetStateText(GuardState));
	}

	if (Role == ROLE_Authority)
	{
		DrawDebugString(GetWorld(), FVector(200), GetStateText(GuardState), this, FColor::Green, 20.f);
		UE_LOG(LogTemp, Warning, TEXT("Authority = %s"), *GetStateText(GuardState));
	}*/

}

void AFPSAIGuard::MoveNextPatrolPoint()
{

	PatrolIndex = PatrolIndex % PatrolPoints.Num();
	CurrentPatrolPoint = PatrolPoints[PatrolIndex];

	UNavigationSystem::SimpleMoveToActor(GetController(), CurrentPatrolPoint);

	PatrolIndex++;
	bCanTick = true;

}



