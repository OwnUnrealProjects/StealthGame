// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AFPSPlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Possess(APawn* aPawn) override;

	//virtual void AddPitchInput(float Val) override;


	/* this funtion is called in APlayerController::PostInitializedComponents */
	virtual void InitPlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void OnMissionCompleted(bool bMissionSuccess);

	enum ENetRole GetOwnerCharacterRole() { return OwnerPlayerRole; }
	enum ENetRole GetOwnerCharacterRemotRole() { return OwnerPlayerRemotRole; }

	FString RoleString();
	
	UFUNCTION(BlueprintCallable, Category = "Player Name")
	void SetCharacterName(FString N) { CharacterName = N; }

	UFUNCTION(BlueprintPure, Category = "Player Name")
	FString GetPlayerName() { return CharacterName; }

	float GetMaxAimPrecision() { return MaxAimPrecision; }
	float GetMaxLoud() { return MaxLoud; }

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Possess, BlueprintReadWrite, Category = "Possess Pawn")
	bool bPossess = false;
	UFUNCTION()
	void OnRep_Possess();

	UPROPERTY(Replicated)
	FString CharacterName;

private:

	enum ENetRole OwnerPlayerRole = ENetRole::ROLE_None;
	enum ENetRole OwnerPlayerRemotRole = ENetRole::ROLE_None;
	
	float MaxAimPrecision;
	float MaxLoud;
};
