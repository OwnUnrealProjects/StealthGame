// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FPSInGameMode.generated.h"

class ATargetPoint;
class UUILobby;

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSInGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	
	AFPSInGameMode();

	void CompleteMission();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);

	void MissionFaild(APawn *SeenPawn);

	void StartGame();
	void TraveNewMap(/*FString URL*/);

	UFUNCTION(BlueprintCallable, Category = "Widget")
		void ConnectLobbyWidget(UUILobby* Lobby);

	UFUNCTION(BlueprintCallable, Category = "Player PostLogin")
		int32 GetPlayerNumber() { return NumberofPlayers; }

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

	AActor* NewViewTarget;
	//ATargetPoint* CameraLocation;

	int32 NumberofPlayers = 0;

	UUILobby* UILobby;

	FTimerHandle TimerHandle_StartGame;

	FTimerHandle TimerHandle_LoadMap;
	
	
};
