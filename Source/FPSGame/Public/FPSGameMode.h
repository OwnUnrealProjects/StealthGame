// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

class ATargetPoint;

UCLASS()
class AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFPSGameMode();
	// TODO not need this Argument
	void CompleteMission();

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);

	void MissionFaild(APawn *SeenPawn);

public:

	virtual void BeginPlay() override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Spectating")
	TSubclassOf<AActor> BPTarget;

	UPROPERTY(EditDefaultsOnly, Category = "Spectating")
	TSubclassOf<AActor> BPObjective;

	/*UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<ATargetPoint> Location;*/

private:

	AActor* NewViewTarget;
	//ATargetPoint* CameraLocation;

};



