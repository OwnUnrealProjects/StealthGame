// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "FPSInGameState.generated.h"

class AFPSGameObject;

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSInGameState : public AGameState
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	//virtual void PostInitializeComponents() override;

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastOnMissionComplite(AActor *CameraNewTarget);
	

	UFUNCTION(NetMulticast, Reliable)
	void MissionFailed(AActor *CameraNewTarget, APawn *InstigatorPawn, bool MissionFail);

	UFUNCTION(BlueprintCallable, Category = "Player Number")
	int32 GetPlayerNumber() { return PlayerNumber; }

	void SetPlayerNumbers(int32 PlayerNum) { PlayerNumber = PlayerNum; }


public:

	/// GameObject System
	UPROPERTY(ReplicatedUsing = OnRep_Game)
	class AFPSGameObject* Game;
	UFUNCTION(BlueprintImplementableEvent, Category = "UnrealGames")
	void OnRep_Game();


	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "UnrealGames")
	class AFPSGameObject* GetGame();


	void SetGame(AFPSGameObject* Game);


private:

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	int32 PlayerNumber;

protected:

	UFUNCTION()
	void UnPossessedPawn(AFPSPlayerController *PC);
	
	
};
