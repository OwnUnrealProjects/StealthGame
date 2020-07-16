// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSInGameInstance.h"
#include "FPSInGameMode.h"
#include "FPSGameObject.h"
#include "../DebugTool/DebugLog.h"


UFPSInGameInstance::UFPSInGameInstance()
{
	Game = CreateDefaultSubobject<AFPSGameObject>(TEXT("GameObject"));
}

void UFPSInGameInstance::StoreUGGame(/*AFPSGameObject* _Game*/)
{
	//Game = _Game;

	auto World = GetWorld();
	if (!IsValid(World)) return;
	auto GM = Cast<AFPSInGameMode>(World->GetAuthGameMode());
	if (!IsValid(GM)) return;
	GM->SetGame(Game);
	TestPlayerState;
	LOG_S("   ")
}

AGameModeBase* UFPSInGameInstance::CreateGameModeForURL(FURL InURL)
{
	AGameModeBase * DefaultGameMode = Super::CreateGameModeForURL(InURL);

	return DefaultGameMode;
}
