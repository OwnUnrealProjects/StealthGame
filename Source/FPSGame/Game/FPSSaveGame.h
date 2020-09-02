// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FPSCommon.h"
#include "FPSSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UFPSSaveGame : public USaveGame
{
	GENERATED_BODY()
	
	
public:

	UPROPERTY()
	TMap<int32, FPlayerData> PlayerStatesData;

	static UFPSSaveGame* CreateSaveGameObject();

	static void SaveGameObject(UFPSSaveGame* SaveObject);

	static UFPSSaveGame* LoadSaveGameObject();

	static FString SlotName;
	
};
