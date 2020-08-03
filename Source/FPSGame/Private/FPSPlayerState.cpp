// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerState.h"
#include "../Game/FPSInGameInstance.h"
#include "../DebugTool/DebugLog.h"
#include "../Game/FPSGameObject.h"
#include "FPSGameState.h"

#include "Net/UnrealNetwork.h"




AFPSPlayerState::AFPSPlayerState() : APlayerState()
{
	//bUseCustomPlayerNames = true;
	//SetReplicates(true);
	PrimaryActorTick.bCanEverTick = true;
}

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	//LOG_S(FString::Printf(TEXT("PlayerName = %s"), *GetPlayerName()));

	if (Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Error, TEXT("BeginePlaye Authority !!!!!!!!!!!!!!!!!!!!"));
	}
	else if (Role == ROLE_AutonomousProxy)
	{
		UE_LOG(LogTemp, Error, TEXT("BeginePlaye AuthonomousProxy !!!!!!!!!!!!!!!!!!!!"));
	}

	//SetPlayerData();
	
}


void AFPSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerState, OwnerPlayerName);
	DOREPLIFETIME(AFPSPlayerState, PlayerScore);

}

void AFPSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	AFPSPlayerState* PS = Cast<AFPSPlayerState>(PlayerState);

	if (PS)
	{
		PS->OwnerPlayerName = this->OwnerPlayerName;
		PS->PlayerScore = this->PlayerScore;
	}

	LOG_S(FString("CopyProperties Executed !!!!!!!!!!"));

	if (GetRemoteRole() == ROLE_AutonomousProxy)
	{
		//PS->PlayerName = this->PlayerName;
		LOG_S(FString("AuthonomousProxy Remote"));
	}

	if (Role == ROLE_Authority)
	{
		LOG_S(FString("Authrity"));
	}
	if (Role == ROLE_AutonomousProxy)
	{
		LOG_S(FString("AuthonomousProxy"));
	}


}


void AFPSPlayerState::SetOwnerPlayerName(FString name)
{
	SetPlayerName(name);
	OwnerPlayerName = name;
	//PlayerName = name;
}




void AFPSPlayerState::OverrideWith(APlayerState* PlayerState)
{
	Super::OverrideWith(PlayerState);

	AFPSPlayerState* PS = Cast<AFPSPlayerState>(PlayerState);

	if (PS)
	{
		OwnerPlayerName = PS->OwnerPlayerName;
	}
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



void AFPSPlayerState::SetPlayerData()
{
	if (!GetOwner()) return;

	auto GI = Cast<UFPSInGameInstance>(GetGameInstance());
	if (GI)
	{
		auto ValidID = GI->Game->PlayerStatesData.Find(PlayerId);
		if (ValidID)
		{
			SetPlayerName(GI->Game->PlayerStatesData[PlayerId].PlayerPawnName);
			Score = GI->Game->PlayerStatesData[PlayerId].ScorePawn;
		}
		else
		{
			GI->Game->PlayerStatesData.Remove(PlayerId);
		}
	}
}