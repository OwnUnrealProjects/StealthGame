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
	
//public:
//
//	virtual void BeginPlay() override;
//
//	UFUNCTION(NetMulticast, Reliable)
//	void MultiCastOnMissionComplite(AActor *CameraNewTarget);
//	
//	UFUNCTION(NetMulticast, Reliable)
//	void MissionFailed(AActor *CameraNewTarget, APawn *InstigatorPawn, bool MissionFail);
//
//	UFUNCTION(BlueprintCallable, Category = "Player Number")
//	int32 GetPlayerNumber()  { return PlayerNumber; }
//
//	void SetPlayerNumbers(int32 PlayerNum) { PlayerNumber = PlayerNum; }
//
//	
//		
//private:
//
//	FTimerDelegate TimerDel;
//	FTimerHandle TimerHandle;
//
//	int32 PlayerNumber;
//
//protected:
//
//	UFUNCTION()
//	void UnPossessedPawn(AFPSPlayerController *PC);

};
