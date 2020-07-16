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

	/* this funtion is called in APlayerController::PostInitializedComponents */
	virtual void InitPlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void OnMissionCompleted(bool bMissionSuccess);

	enum ENetRole GetOwnerCharacterRole() { return OwnerPlayerRole; }
	enum ENetRole GetOwnerCharacterRemotRole() { return OwnerPlayerRemotRole; }

	FString RoleString();

	void SetPlayerName(FString Name);



protected:

	UPROPERTY(ReplicatedUsing = OnRep_Possess, BlueprintReadWrite, Category = "Possess Pawn")
	bool bPossess = false;
	UFUNCTION()
	void OnRep_Possess();

private:

	enum ENetRole OwnerPlayerRole = ENetRole::ROLE_None;
	enum ENetRole OwnerPlayerRemotRole = ENetRole::ROLE_None;
	
	
};
