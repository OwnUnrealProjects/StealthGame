// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSInGameInstance.h"




void UFPSInGameInstance::NewPlayer(AFPSPlayerState* Player)
{
	Players.AddUnique(Player);	
}

void UFPSInGameInstance::UpdatePlayers(AFPSPlayerState* OldPS, AFPSPlayerState* NewPS)
{
	if (OldPS == nullptr || NewPS == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UGGame::UpdatePlayerState: Invalid argument."));
		return;
	}

	int32 i = Players.Find(OldPS);
	if (i != INDEX_NONE)
	{
		Players.Remove(OldPS);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("UGGame::UpdatePlayerState: OldPS (%s) is not in game."), *GetNameSafe(OldPS));
		return;
	}

	Players.Add(NewPS);
}
