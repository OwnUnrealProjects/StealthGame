// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSInMenuGameMode.h"
#include "UObject/ConstructorHelpers.h"

#include "../Public/FPSPlayerController.h"
#include "../Game/FPSInGameState.h"
#include "../Public/FPSPlayerState.h"
#include "../Public/FPSCharacter.h"
#include "../Public/FPSHUD.h"




AFPSInMenuGameMode::AFPSInMenuGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	//bUseSeamlessTravel = true; // In First Level this not be true

	// use our custom HUD class
	/*HUDClass = AFPSHUD::StaticClass();
	GameStateClass = AFPSInGameState::StaticClass();
	PlayerStateClass = AFPSPlayerState::StaticClass();
	PlayerControllerClass = AFPSPlayerController::StaticClass();*/
	//DefaultPawnClass = AFPSCharacter::StaticClass();
	//bUseSeamlessTravel = false;
}


void AFPSInMenuGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);




}