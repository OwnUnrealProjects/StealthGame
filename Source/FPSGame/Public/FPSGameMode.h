// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSavePlayerStatesData);

class ATargetPoint;
class UUILobby;

UCLASS()
class AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFPSGameMode();
	
	void CompleteMission();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);

	void MissionFaild(APawn *SeenPawn);

	UFUNCTION(BlueprintCallable, Category = "FPSGameMode")
	void StartGame();

	void TraveNewMap(/*FString URL*/);

	UFUNCTION(BlueprintCallable, Category = "Widget")
	void ConnectLobbyWidget(UUILobby* Lobby);

	UFUNCTION(BlueprintCallable, Category = "Player PostLogin")
	int32 GetPlayerNumber() { return NumberofPlayers; }

	UPROPERTY()
	FSavePlayerStatesData SavePlayerStatesData;

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

	class AFPSGameState* GS;

	AActor* NewViewTarget;
	//ATargetPoint* CameraLocation;

	int32 NumberofPlayers = 0;

	UUILobby* UILobby;

	FTimerHandle TimerHandle_StartGame;
	FTimerHandle TimerHandle_LoadMap;

	
};



