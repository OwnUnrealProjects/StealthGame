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

	UFUNCTION(NetMulticast, Reliable)
	void MultiCastOnMissionComplite(AActor *CameraNewTarget);
	
	UFUNCTION(NetMulticast, Reliable)
	void MissionFailed(AActor *CameraNewTarget, APawn *InstigatorPawn, bool MissionFail);
	
private:

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle;

protected:

	UFUNCTION()
	void UnPossessedPawn(AFPSPlayerController *PC);

};
