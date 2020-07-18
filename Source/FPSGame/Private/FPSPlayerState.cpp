// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerState.h"
#include "../Game/FPSInGameInstance.h"
#include "../DebugTool/DebugLog.h"
#include "../Game/FPSGameObject.h"




AFPSPlayerState::AFPSPlayerState()
{
	//bUseCustomPlayerNames = true;

}

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	LOG_S(FString::Printf(TEXT("PlayerName = %s"), *GetPlayerName()));
}

void AFPSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	AFPSPlayerState* PS = Cast<AFPSPlayerState>(PlayerState);

	/*UFPSInGameInstance* GI = Cast<UFPSInGameInstance>(GetGameInstance());
	if (GI)
	{
		GI->Game->UpdatePlayers(this, PS);
		GI->TestPlayerState = PS;
		LOG_S(FString::Printf(TEXT("TestPlayerState Name = %s"), *GI->TestPlayerState->GetPlayerName()));
		LOG_S(this->GetPlayerName());
		LOG_S(PlayerState->GetPlayerName());

	}*/

	

}
