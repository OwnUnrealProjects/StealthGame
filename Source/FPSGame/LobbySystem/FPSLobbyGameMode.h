// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Game/FPSCommon.h"
#include "FPSLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFPSLobbyGameMode();
	
protected:

	TMap<ETypeOfPawn, UClass*> PawnType;

protected:
	
	// virtual UClass* GetDefaultPawnClassForController(AController* InController) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	UFUNCTION(BlueprintCallable, Category = "Start Game")
	void StartGamee();
	
};
