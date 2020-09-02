// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCommon.generated.h"

UENUM(BlueprintType)
enum class ETypeOfPawn : uint8
{
	Aj    = 0,
	Kaya  = 1
};


USTRUCT(BlueprintType, Blueprintable)
struct FPlayerData
{
	GENERATED_USTRUCT_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerData")
	ETypeOfPawn PawnType;

	UPROPERTY(BlueprintReadWrite, Category = "PlayerData")
	FString PlayerPawnName;
	
	UPROPERTY(BlueprintReadWrite, Category = "PlayerData")
	int32 ScorePawn;


};