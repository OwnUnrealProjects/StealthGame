// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuBase.generated.h"

class UFPSGameInstance;

/**
 * 
 */
UCLASS()
class FPSGAME_API UMenuBase : public UUserWidget
{
	GENERATED_BODY()	
	
public:

	void SetUIMode();
	void SetUIScreen();
	void SetGameMode();

	void SetFPSGameInstacne(UFPSGameInstance *GI) { GameInstance = GI; }

	const UFPSGameInstance* GetFPSGameInstance() { return GameInstance; }

protected:

	static UFPSGameInstance *GameInstance;

	
};
