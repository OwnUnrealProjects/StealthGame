// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FPSGameState.h"
#include "FPSObjectiveActor.h"
#include "FPSPlayerController.h"
#include "../Game/FPSGameObject.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TargetPoint.h"
#include "FPSPlayerController.h"
#include "../UI/UILobby.h"
#include "../MenuSystem/FPSGameInstance.h"
#include "FPSPlayerState.h"
#include "../Game/FPSInGameInstance.h"
#include "../DebugTool/DebugLog.h"



AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
	GameStateClass = AFPSGameState::StaticClass();
	PlayerStateClass = AFPSPlayerState::StaticClass();
	PlayerControllerClass = AFPSPlayerController::StaticClass();
}



void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay Set GS"));

	GS = GetGameState<AFPSGameState>();

	GS->SetPlayerNumbers(NumberofPlayers);
	
}



void AFPSGameMode::CompleteMission()
{

		/// if check DisableInput() with buttons Alt + G two times
		/// we can see it's parameter may be PlayerController or NULL
	
		TArray<AActor*> BP_ClassArray;
		UGameplayStatics::GetAllActorsOfClass(this, BPTarget, BP_ClassArray);

		if (BP_ClassArray.Num() > 0)
		{
			NewViewTarget = BP_ClassArray[0];
			//UE_LOG(LogTemp, Warning, TEXT("BP_BlueprintArray Number = %i"), BP_ClassArray.Num());

		}

		/// TSubClassOf<>() can not access the class which is created already
		/// in this cases CameraLocatoin must be created in the world for get him Location()
		/// but instead of CameraLocation we can use FVector & FRotator directly
		

		//AFPSGameState* GS = GetGameState<AFPSGameState>();
		if (GS)
		{
			GS->MultiCastOnMissionComplite(NewViewTarget);
			UE_LOG(LogTemp, Warning, TEXT("GameState Is not NULL"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameState Is NULL"));
		}

		GetWorldTimerManager().SetTimer(TimerHandle_LoadMap, this, &AFPSGameMode::TraveNewMap, 4.f);
	

	//OnMissionCompleted(InstigatorPawn, bMissionSuccess);

}


void AFPSGameMode::MissionFaild(APawn * SeenPawn)
{
	TArray<AActor*> BP_ClassArray;
	UGameplayStatics::GetAllActorsOfClass(this, BPTarget, BP_ClassArray);

	if (BP_ClassArray.Num() > 0)
	{
		NewViewTarget = BP_ClassArray[0];
		//UE_LOG(LogTemp, Warning, TEXT("BP_BlueprintArray Number = %i"), BP_ClassArray.Num());

	}

	//auto SeenPlayer = Cast<AFPSCharacter>(SeenPawn);
	//if (SeenPlayer->bIsCarryingObjective)
	//{
	//	//Set Spawn Collision Handling Override
	//	FActorSpawnParameters ActorSpawnParams;
	//	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//	//ActorSpawnParams.Instigator = this;

	//	FVector location = SeenPawn->GetActorLocation();

	//	auto Objective = GetWorld()->SpawnActor<AFPSObjectiveActor>(BPObjective, location, FRotator::ZeroRotator, ActorSpawnParams);
	//	if (Objective)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Objective Name = %s"), *Objective->GetName());
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Objective is NULL"));
	//	}
	//}

	//GS = GetGameState<AFPSGameState>();
	if (GS)
	{
		GS->MissionFailed(NewViewTarget, SeenPawn, false);
		UE_LOG(LogTemp, Warning, TEXT("GameState Is not NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState Is NULL"));
	}
}


/// ===========================================================
/// Level Load

void AFPSGameMode::PostLogin(APlayerController* NewPlayer)
{

	Super::PostLogin(NewPlayer);


	++NumberofPlayers;

	/*AFPSPlayerState* PS = Cast<AFPSPlayerState>(NewPlayer->PlayerState);
	UFPSInGameInstance* GI = Cast<UFPSInGameInstance>(GetGameInstance());
	if (PS && GI)
	{
		if(GI->PlayerNumber == 0) GI->PlayerNumber = NumberofPlayers;
		if (GI->Game->PlayerStatesData.Num() != 0)
		{
			auto ValidKey = GI->Game->PlayerStatesData.Find(PS->PlayerId);
			if (ValidKey)
			{
				FPlayerData PlayerData = GI->Game->PlayerStatesData[PS->PlayerId];
				PS->SetPlayerName(PlayerData.PlayerPawnName);
				PS->Score = PlayerData.ScorePawn;
			}
	
		}
	}

	UE_LOG(LogTemp, Error, TEXT("GameMode PostLogin !!!!!!!!!!!!!!!!!!!!!!"));*/

}

void AFPSGameMode::ConnectLobbyWidget(UUILobby* Lobby)
{
	UILobby = Lobby;
	UILobby->Start.AddDynamic(this, &AFPSGameMode::StartGame);
}

void AFPSGameMode::StartGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	//SavePlayerStatesData.Broadcast();

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/FirstPersonExampleMap?listen");

	// Get GameInstance
	auto GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	GameInstance->StartSession();

	//GetWorldTimerManager().ClearTimer(TimerHandle_StartGame);
}


void AFPSGameMode::TraveNewMap(/*FString URL*/)
{

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/City?listen");

	LOG_S(FString("TraveNewMap ServerTravel"));
}



/// Other Version
//void AFPSGameMode::TraveNewMap(FString URL)
//{
//	
//	int32 index = 0;
//	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
//	{
//		LOG_I(index);
//		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
//		/*if (!PC) continue;*/
//
//		if (PC->GetOwnerCharacterRemotRole() == ROLE_SimulatedProxy)
//		{
//			UWorld *World = GetWorld();
//			if (!ensure(World != nullptr)) return;
//			World->ServerTravel("/Game/Maps/City?listen", true, true);
//			LOG_S(FString("TraveNewMap ServerTravel"));
//		}
//		else if(PC->GetOwnerCharacterRemotRole() == ROLE_AutonomousProxy)
//		{
//			PC->ClientTravel("127.0.0.1", ETravelType::TRAVEL_Absolute);
//			LOG_S(FString("TraveNewMap ClientTravel"));
//		}
//
//		
//		index++;
//	}
//
//}