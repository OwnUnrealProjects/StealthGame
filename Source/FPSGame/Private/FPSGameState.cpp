// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "FPSPlayerController.h"



void AFPSGameState::MultiCastOnMissionComplite_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	/*for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* Pawn = It->Get();
		if (Pawn && Pawn->IsLocallyControlled())
		{
			Pawn->DisableInput(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("MultiCastOnMissionComplite Implementation"));
		}
	}*/

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController())
		{
			PC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);
			PC->GetPawn()->DisableInput(PC);
		}
	}

}
