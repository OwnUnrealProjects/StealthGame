// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSInGameState.h"
#include "../Public/FPSCharacter.h"
#include "../public/FPSPlayerController.h"
#include "../DebugTool/DebugLog.h"



void AFPSInGameState::BeginPlay()
{
	Super::BeginPlay();
	LOG_S(FString(" GameState"));
}


void AFPSInGameState::MultiCastOnMissionComplite_Implementation(AActor *CameraNewTarget)
{


	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{

		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController() && PC->GetPawn())
		{
			auto Player = Cast<AFPSCharacter>(PC->GetPawn());
			UE_LOG(LogTemp, Warning, TEXT("GameState Player name = %s,  CarryingObjective = %i"), *Player->GetName(), Player->bIsCarryingObjective);
			PC->SetViewTargetWithBlend(CameraNewTarget, 1.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			PC->OnMissionCompleted(Player->bIsCarryingObjective);
			PC->GetPawn()->DisableInput(PC);


			//TimerDel.BindUFunction(this, FName("UnPossessedPawn"), PC);
			//GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 3.f, false);
			//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFPSInGameState::UnPossessedPawn, 3.f, false);
		}

	}

	/*for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* Pawn = It->Get();
		if (Pawn && Pawn->IsLocallyControlled())
		{
			Pawn->DisableInput(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("MultiCastOnMissionComplite Implementation"));
		}
	}*/



}

void AFPSInGameState::MissionFailed_Implementation(AActor * CameraNewTarget, APawn * InstigatorPawn, bool MissionFail)
{

	auto Player = Cast<AFPSCharacter>(InstigatorPawn);
	if (!Player) return;

	UE_LOG(LogTemp, Warning, TEXT("GameState MissionFailed Player Name = %s"), *InstigatorPawn->GetName());

	AFPSPlayerController* PC = Cast<AFPSPlayerController>(Player->GetController());
	if (!PC) return;

	if (PC->IsLocalController())
	{
		PC->SetViewTargetWithBlend(CameraNewTarget, 1.5f, EViewTargetBlendFunction::VTBlend_Cubic);
		PC->OnMissionCompleted(false);
		PC->GetPawn()->DisableInput(PC);
		UE_LOG(LogTemp, Warning, TEXT("GameState Unpossesed Block"));
	}

	TimerDel.BindUFunction(this, FName("UnPossessedPawn"), PC);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 3.f, false);


}

void AFPSInGameState::UnPossessedPawn(AFPSPlayerController *PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Timer"));
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	PC->UnPossess();
}



