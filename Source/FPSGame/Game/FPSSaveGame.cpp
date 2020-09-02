// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSSaveGame.h"
#include "Kismet/GameplayStatics.h"


FString UFPSSaveGame::SlotName = "MySlot";

UFPSSaveGame* UFPSSaveGame::CreateSaveGameObject()
{
	UFPSSaveGame* SaveGameInstance = Cast<UFPSSaveGame>(UGameplayStatics::CreateSaveGameObject(UFPSSaveGame::StaticClass()));
	return SaveGameInstance;
}

void UFPSSaveGame::SaveGameObject(UFPSSaveGame* SaveObject)
{
	UGameplayStatics::SaveGameToSlot(SaveObject, SlotName, 0);
}

UFPSSaveGame* UFPSSaveGame::LoadSaveGameObject()
{
	UFPSSaveGame* SaveGameInstance = Cast<UFPSSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	return SaveGameInstance;
}
