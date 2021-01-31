// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../LobbySystem/FPSLobbyGameMode.h"
#include "FPSGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSavePlayerStatesData);

class ATargetPoint;
class UUILobby;
class AFPSPlayerController;

UCLASS()
class AFPSGameMode : public AFPSLobbyGameMode
{
	GENERATED_BODY()

public:

	AFPSGameMode();
	
	void CompleteMission();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);

	void MissionFaild(APawn *SeenPawn);


public:

	virtual void BeginPlay() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:

	/* Camera Target */
	UPROPERTY(EditDefaultsOnly, Category = "Spectating")
	TSubclassOf<AActor> BPTarget;

	/*UPROPERTY(EditDefaultsOnly, Category = "Spectating")
	TSubclassOf<AActor> BPObjective;*/

	/*UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<ATargetPoint> Location;*/

private:

	class AFPSGameState* GS;

	AActor* NewViewTarget;
	//ATargetPoint* CameraLocation;


/// Data Travel System
public:

	UFUNCTION(BlueprintCallable, Category = "FPSGameMode")
	void StartGame();

	void TraveNewMap(/*FString URL*/);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void ConnectLobbyWidget(UUILobby* Lobby);

	UFUNCTION(BlueprintCallable, Category = "Player PostLogin")
	int32 GetPlayerNumber() { return NumberofPlayers; }

	UPROPERTY()
	FSavePlayerStatesData SavePlayerStatesData;

protected:
	
	void SavePlayerData();

protected:


	UUILobby* UILobby;

	FTimerHandle TimerHandle_StartGame;
	FTimerHandle TimerHandle_LoadMap;

public:

	TArray<AFPSPlayerController*> ConnectPlayerControllers;

	int32 NumberofPlayers = 0;

	
};



