// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

class AFPSPlayerController;

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:

	AFPSGameState();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;


	UFUNCTION(NetMulticast, Reliable)
	void MultiCastOnMissionComplite(AActor *CameraNewTarget);
	
	UFUNCTION(NetMulticast, Reliable)
	void MissionFailed(AActor *CameraNewTarget, APawn *InstigatorPawn, bool MissionFail);

	

	
		
private:

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

	int32 PlayerNumber;

protected:

	UFUNCTION()
	void UnPossessedPawn(AFPSPlayerController *PC);

/// Data Travel System
protected:

	UFUNCTION()
	void SavePlayerStatesData();

	UFUNCTION(BlueprintCallable, Category = "Player Number")
	int32 GetPlayerNumber() { return PlayerNumber; }


public:

	void AddPlayerState(APlayerState* PlayerState);

	void SetPlayerNumbers(int32 PlayerNum) { PlayerNumber = PlayerNum; }

	UFUNCTION(NetMulticast, Reliable)
	void SavePlayerData(const TArray<AFPSPlayerController*> &ConnectPC);


};
