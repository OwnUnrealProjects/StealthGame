// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerState.h"
//#include "../Game/FPSInGameMode.h"
#include "../Public/FPSGameMode.h"
#include "../Game/FPSInGameInstance.h"
#include "../Game/FPSGameObject.h"
#include "../DebugTool/DebugLog.h"

AFPSPlayerController::AFPSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//OwnerPlayerRole = GetPawn()->Role;
	
	/*FString Name;
	GetName(Name);
	LOG_S(FString::Printf(TEXT("Own Name = %s   PlayerState PlayerName = %s"), *Name, *PlayerState->GetPlayerName()));*/

	
}

void AFPSPlayerController::Tick(float DeltaSeconds)
{
	
	Super::Tick(DeltaSeconds);

	FString Name;

	auto PS = Cast<AFPSPlayerState>(PlayerState);
	if(!PS) return;


	//GetName(Name);
	//LOG_S(FString::Printf(TEXT("Own Name = %s   PlayerState PlayerName = %s, OwnScore = %i"), *PS->GetOwnerPlayerName(), *PS->GetPlayerName(), PS->GetOwnerPlayerScore()));
	//LOG_S(PlayerState->GetName());
	//LOG_I(PlayerState->Score);
}

void AFPSPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	OwnerPlayerRole = aPawn->Role;
	OwnerPlayerRemotRole = aPawn->GetRemoteRole();
	bPossess = true;
}



//void AFPSPlayerController::AddPitchInput(float Val)
//{
//	Super::AddPitchInput(Val);
//	LOG_F(RotationInput.Pitch);
//}

void AFPSPlayerController::InitPlayerState()
{

	Super::InitPlayerState();

	/*auto GI = Cast<UFPSInGameInstance>(GetGameInstance());
	auto PS = Cast<AFPSPlayerState>(PlayerState);
	if (GI && PS)
	{
		bool validArray = GI->Game->PlayerStatesData.Num() != 0 ? true : false;
		auto ValidKey = GI->Game->PlayerStatesData.Find(PS->PlayerId);

		if (ValidKey && validArray)
		{
			FString name = GI->Game->PlayerStatesData[PS->PlayerId].PlayerPawnName;
			auto score = GI->Game->PlayerStatesData[PS->PlayerId].ScorePawn;
			PlayerState->SetPlayerName(name);
			PlayerState->Score = score;
		}
		
	}*/

	//if (GetNetMode() != NM_Client)
	//{
	//	UWorld* const World = GetWorld();
	//	AFPSGameMode* const GameMode = World ? Cast<AFPSGameMode>(World->GetAuthGameMode()) : NULL;
	//	//LOG_S(FString::Printf(TEXT("GameMode Name = %s"), *GameMode->TestGameModeName));  // Crash the Editor
	//	//GameMode->TestGameModeName = "GM";
	//	//LOG_S(FString::Printf(TEXT("GameMode Name = %s"), *GameMode->TestGameModeName));
	//	if (GameMode != NULL)
	//	{
	//		FActorSpawnParameters SpawnInfo;
	//		SpawnInfo.Owner = this;
	//		SpawnInfo.Instigator = Instigator;
	//		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//		SpawnInfo.ObjectFlags |= RF_Transient;	// We never want player states to save into a map
	//		PlayerState = World->SpawnActor<APlayerState>(GameMode->GetPlayerStateClass(), SpawnInfo);
	//		

	//		// force a default player name if necessary
	//		if (PlayerState && PlayerState->GetPlayerName().IsEmpty())
	//		{
	//			// don't call SetPlayerName() as that will broadcast entry messages but the GameMode hasn't had a chance
	//			// to potentially apply a player/bot name yet
	//			PlayerState->GetPlayerName() = GameMode->DefaultPlayerName.ToString();
	//		}
	//	}
	//	else
	//	{
	//		Super::InitPlayerState();
	//	}
	//}
}

FString AFPSPlayerController::RoleString()
{
	return "None";
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
	DOREPLIFETIME(AFPSPlayerController, CharacterName);
}