// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/FPSGameInstance.h"
#include "FPSInGameInstance.generated.h"

class AFPSPlayerState;

/**
 * 
 */
UCLASS()
class FPSGAME_API UFPSInGameInstance : public UFPSGameInstance
{
	GENERATED_BODY()
	
	
public:

	// Clients who are actually in game (not spectators or whatever)
	UPROPERTY(BlueprintReadOnly)
	TArray<AFPSPlayerState*> Players;

	int32 PlayerNumber;

public:
	
	UFUNCTION(BlueprintCallable, Category = "InGameInstance")
	void NewPlayer(AFPSPlayerState* Player);

	void UpdatePlayers(AFPSPlayerState* OldPS, AFPSPlayerState* NewPS);
	
};
