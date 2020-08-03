// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	//virtual void PostInitializeComponents() override;

	AFPSPlayerState();

	virtual void BeginPlay() override;

	void SetPlayerData();

	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, Category = "FPSPlayerState")
	void SetOwnerPlayerName(FString name);

	UFUNCTION(BlueprintPure, Category = "FPSPlayerState")
	FString GetOwnerPlayerName() { return OwnerPlayerName; }


	UFUNCTION(BlueprintCallable, Category = "FPSPlayerState")
	void SetOwnerPlayerScore(int32 ownscore) { Score = ownscore; PlayerScore = ownscore; }

	UFUNCTION(BlueprintPure, Category = "FPSPlayerState")
	int32 GetOwnerPlayerScore() { return PlayerScore; }



protected:
	
	UPROPERTY(BlueprintReadOnly,Replicated, Category = "Player Own Name")
	FString OwnerPlayerName;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Player Own Name")
	int32 PlayerScore;
	
};
