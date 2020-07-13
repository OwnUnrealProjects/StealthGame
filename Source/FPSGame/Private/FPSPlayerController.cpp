// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerState.h"
#include "../Game/FPSInGameMode.h"
#include "../DebugTool/DebugLog.h"

AFPSPlayerController::AFPSPlayerController()
{

}


void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//OwnerPlayerRole = GetPawn()->Role;
}

void AFPSPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	OwnerPlayerRole = aPawn->Role;
	OwnerPlayerRemotRole = aPawn->GetRemoteRole();
	bPossess = true;
}




void AFPSPlayerController::InitPlayerState()
{
	if (GetNetMode() != NM_Client)
	{
		UWorld* const World = GetWorld();
		AFPSInGameMode* const GameMode = World ? Cast<AFPSInGameMode>(World->GetAuthGameMode()) : NULL;
		if (GameMode != NULL)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Owner = this;
			SpawnInfo.Instigator = Instigator;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnInfo.ObjectFlags |= RF_Transient;	// We never want player states to save into a map
			PlayerState = World->SpawnActor<APlayerState>(GameMode->PlayerStateClass, SpawnInfo);

			// force a default player name if necessary
			if (PlayerState && PlayerState->GetPlayerName().IsEmpty())
			{
				// don't call SetPlayerName() as that will broadcast entry messages but the GameMode hasn't had a chance
				// to potentially apply a player/bot name yet
				PlayerState->GetPlayerName() = GameMode->DefaultPlayerName.ToString();
			}
		}
		else
		{
			Super::InitPlayerState();
		}
	}
}

FString AFPSPlayerController::RoleString()
{
	return "None";
}

void AFPSPlayerController::SetPlayerName(FString Name)
{
	AFPSPlayerState* PS = Cast<AFPSPlayerState>(PlayerState);
	if (PS)
	{
		//LOG_S(PS->GetPlayerName());
		PS->SetOwnerPlayerName(Name);
		LOG_S(FString("FPSPlayerState is Not NULL"));
		//LOG_S(PS->GetPlayerName());
	}
	else
	{
		LOG_S(FString("FPSPlayerState is NULL"));
	}

	/*AFPSInGameMode* GM = Cast<AFPSInGameMode>(GetWorld()->GetAuthGameMode());
	GM->ChangeName(this, Name, true);*/

	
		
	LOG_S(Name);
}

void AFPSPlayerController::OnRep_Possess()
{
	OwnerPlayerRole = ENetRole::ROLE_AutonomousProxy;
	OwnerPlayerRemotRole = ENetRole::ROLE_Authority;
}

void AFPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerController, bPossess);
}