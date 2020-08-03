// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/FPSGameInstance.h"
#include "FPSInGameInstance.generated.h"

class AFPSPlayerState;
class AFPSGameObject;

/**
 * 
 */
UCLASS()
class FPSGAME_API UFPSInGameInstance : public UFPSGameInstance
{
	GENERATED_BODY()
	
	
public:

	UFPSInGameInstance();

	UPROPERTY(VisibleAnywhere, Category = "GameObject")
	AFPSGameObject* Game;

	// Used to store the unreal game reference during travel
	UFUNCTION(BlueprintCallable, Category = "UnrealGames")
	void StoreUGGame(/*AFPSGameObject* _Game*/);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "PlayerController")
	int32 PlayerNumber;

	/** Call to create the game mode for a given map URL */
	virtual class AGameModeBase* CreateGameModeForURL(FURL InURL) override;

	UPROPERTY(BlueprintReadWrite)
	AFPSPlayerState* TestPlayerState;


};
