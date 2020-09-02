// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerState.h"
#include "../Game/FPSInGameInstance.h"
#include "../DebugTool/DebugLog.h"
#include "../Game/FPSGameObject.h"
#include "FPSGameState.h"

#include "Net/UnrealNetwork.h"




AFPSPlayerState::AFPSPlayerState() : APlayerState()
{
	
}

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();
	
	/// This function run only Server
	
}


void AFPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerState, CurrentPlayerData);

}

void AFPSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	AFPSPlayerState* PS = Cast<AFPSPlayerState>(PlayerState);

	if (PS)
	{
		PS->CurrentPlayerData = this->CurrentPlayerData;
	}
	
	/// This function run only Server


}


void AFPSPlayerState::SetOwnerPlayerName(FString name)
{
	CurrentPlayerData.PlayerPawnName = name;
}




void AFPSPlayerState::SetOwnerPlayerData(FPlayerData Data)
{
	CurrentPlayerData.PawnType = Data.PawnType;
	CurrentPlayerData.PlayerPawnName = Data.PlayerPawnName;
	SetOwnerPlayerScore(Data.ScorePawn);
}

void AFPSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();


	UWorld* World = GetWorld();
	if(!World) return;

	auto GS = Cast<AFPSGameState>(World->GetGameState());

	if (GS != NULL)
	{
		GS->AddPlayerState(this);
	}
}


/// No more need
//void AFPSPlayerState::SetPlayerData()
//{
//	if (!GetOwner()) return;
//
//	auto GI = Cast<UFPSInGameInstance>(GetGameInstance());
//	if (GI)
//	{
//		auto ValidID = GI->Game->PlayerStatesData.Find(PlayerId);
//		if (ValidID)
//		{
//			SetPlayerName(GI->Game->PlayerStatesData[PlayerId].PlayerPawnName);
//			Score = GI->Game->PlayerStatesData[PlayerId].ScorePawn;
//		}
//		else
//		{
//			GI->Game->PlayerStatesData.Remove(PlayerId);
//		}
//	}
//}