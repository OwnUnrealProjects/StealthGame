// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerState.h"
#include "../Game/FPSInGameInstance.h"
#include "../DebugTool/DebugLog.h"




AFPSPlayerState::AFPSPlayerState()
{
	//bUseCustomPlayerNames = true;

}

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	LOG_S(GetPlayerName());
}

void AFPSPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	AFPSPlayerState* PS = Cast<AFPSPlayerState>(PlayerState);
	PS->SetPlayerNameInternal(this->GetPlayerName());

	UFPSInGameInstance* GI = Cast<UFPSInGameInstance>(GetGameInstance());
	if(GI)
		GI->UpdatePlayers(this, PS);

}
