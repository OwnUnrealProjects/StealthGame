// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FPSInMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSInMenuGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	AFPSInMenuGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};
