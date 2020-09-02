// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../Game/FPSCommon.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	AFPSPlayerState();

	virtual void BeginPlay() override;


	virtual void CopyProperties(APlayerState* PlayerState) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "FPSPlayerState")
	void SetOwnerPlayerName(FString name);

	UFUNCTION(BlueprintCallable, Category = "FPSPlayerState")
	void SetOwnerPlayerData(FPlayerData Data);

	UFUNCTION(BlueprintPure, Category = "FPSPlayerState")
	FString GetOwnerPlayerName() { return CurrentPlayerData.PlayerPawnName; }


	UFUNCTION(BlueprintCallable, Category = "FPSPlayerState")
	void SetOwnerPlayerScore(int32 ownscore) { CurrentPlayerData.ScorePawn = ownscore; }

	UFUNCTION(BlueprintPure, Category = "FPSPlayerState")
	int32 GetOwnerPlayerScore() { return CurrentPlayerData.ScorePawn; }

	// No more need
	//void SetPlayerData();

public:
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "PlayerData")
	FPlayerData CurrentPlayerData;
	
};
