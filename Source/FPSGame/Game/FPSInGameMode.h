// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "../MenuSystem/FPSInMenuGameMode.h"
#include "FPSInGameMode.generated.h"

class ATargetPoint;
class UUILobby;
class AFPSGameObject;

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSInGameMode : public AFPSInMenuGameMode
{
	GENERATED_BODY()
	
public:
	
	AFPSInGameMode();

	void CompleteMission();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);

	void MissionFaild(APawn *SeenPawn);

	UFUNCTION(BlueprintCallable, Category = "FPSInGameMode")
	void StartGame();

	void TraveNewMap(/*FString URL*/);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void ConnectLobbyWidget(UUILobby* Lobby);

	UFUNCTION(BlueprintCallable, Category = "Player PostLogin")
	int32 GetPlayerNumber() { return NumberofPlayers; }


public:

	void SetGame(AFPSGameObject* Game);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UnrealGames")
	AFPSGameObject* GetGame();

	TSubclassOf<APlayerState> GetPlayerStateClass();


public:

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:

	/* Camera Target */
	UPROPERTY(EditDefaultsOnly, Category = "Spectating")
	TSubclassOf<AActor> BPTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Spectating")
	TSubclassOf<AActor> BPObjective;

	/*UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<ATargetPoint> Location;*/

private:

	class AFPSInGameState* GS;
	class UFPSInGameInstance* GI;

	AActor* NewViewTarget;
	//ATargetPoint* CameraLocation;

	int32 NumberofPlayers = 0;

	UUILobby* UILobby;

	FTimerHandle TimerHandle_StartGame;

	FTimerHandle TimerHandle_LoadMap;
	
	
};
