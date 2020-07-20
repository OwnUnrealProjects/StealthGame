// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSInGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FPSInGameState.h"
#include "FPSObjectiveActor.h"
#include "FPSPlayerController.h"
#include "FPSInGameInstance.h"
#include "FPSGameObject.h"
#include "../UI/UILobby.h"
//#include "../MenuSystem/FPSGameInstance.h"
#include "../Public/FPSPlayerState.h"
#include "../DebugTool/DebugLog.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TargetPoint.h"





//AFPSInGameMode::AFPSInGameMode()
//{
//	// set default pawn class to our Blueprinted character
//	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
//	DefaultPawnClass = PlayerPawnClassFinder.Class;
//
//	// use our custom HUD class
//	HUDClass = AFPSHUD::StaticClass();
//	GameStateClass = AFPSInGameState::StaticClass();
//	PlayerStateClass = AFPSPlayerState::StaticClass();
//	PlayerControllerClass = AFPSPlayerController::StaticClass();
//	//bUseSeamlessTravel = true;
//
//	/*auto World = GetWorld();
//	if(!World) return;
//
//	UFPSInGameInstance* GI = Cast<UFPSInGameInstance>(World->GetGameInstance());
//	if (GI)
//	{
//		GI->StoreUGGame();
//	}*/
//}
//
//
//
//void AFPSInGameMode::BeginPlay()
//{
//	Super::BeginPlay();
//
//	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay Set GS"));
//
//	GS = GetGameState<AFPSInGameState>();
//	GS->SetPlayerNumbers(NumberofPlayers);
//
//	/*if (NumberofPlayers > 0)
//		GetWorldTimerManager().SetTimer(TimerHandle_StartGame, this, &AFPSInGameMode::StartGame, 10.f);*/
//}
//
//
//TSubclassOf<APlayerState> AFPSInGameMode::GetPlayerStateClass()
//{
//	return PlayerStateClass;
//}
//
//void AFPSInGameMode::CompleteMission()
//{
//
//	/// if check DisableInput() with buttons Alt + G two times
//	/// we can see it's parameter may be PlayerController or NULL
//
//	TArray<AActor*> BP_ClassArray;
//	UGameplayStatics::GetAllActorsOfClass(this, BPTarget, BP_ClassArray);
//
//	if (BP_ClassArray.Num() > 0)
//	{
//		NewViewTarget = BP_ClassArray[0];
//		//UE_LOG(LogTemp, Warning, TEXT("BP_BlueprintArray Number = %i"), BP_ClassArray.Num());
//
//	}
//
//	/// TSubClassOf<>() can not access the class which is created already
//	/// in this cases CameraLocatoin must be created in the world for get him Location()
//	/// but instead of CameraLocation we can use FVector & FRotator directly
//
//
//	//AFPSInGameState* GS = GetGameState<AFPSInGameState>();
//	if (GS)
//	{
//		GS->MultiCastOnMissionComplite(NewViewTarget);
//		UE_LOG(LogTemp, Warning, TEXT("GameState Is not NULL"));
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("GameState Is NULL"));
//	}
//
//	GetWorldTimerManager().SetTimer(TimerHandle_LoadMap, this, &AFPSInGameMode::TraveNewMap, 4.f);
//
//
//	//OnMissionCompleted(InstigatorPawn, bMissionSuccess);
//
//}
//
//
//void AFPSInGameMode::MissionFaild(APawn * SeenPawn)
//{
//	TArray<AActor*> BP_ClassArray;
//	UGameplayStatics::GetAllActorsOfClass(this, BPTarget, BP_ClassArray);
//
//	if (BP_ClassArray.Num() > 0)
//	{
//		NewViewTarget = BP_ClassArray[0];
//		//UE_LOG(LogTemp, Warning, TEXT("BP_BlueprintArray Number = %i"), BP_ClassArray.Num());
//
//	}
//
//	//auto SeenPlayer = Cast<AFPSCharacter>(SeenPawn);
//	//if (SeenPlayer->bIsCarryingObjective)
//	//{
//	//	//Set Spawn Collision Handling Override
//	//	FActorSpawnParameters ActorSpawnParams;
//	//	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
//	//	//ActorSpawnParams.Instigator = this;
//
//	//	FVector location = SeenPawn->GetActorLocation();
//
//	//	auto Objective = GetWorld()->SpawnActor<AFPSObjectiveActor>(BPObjective, location, FRotator::ZeroRotator, ActorSpawnParams);
//	//	if (Objective)
//	//	{
//	//		UE_LOG(LogTemp, Warning, TEXT("Objective Name = %s"), *Objective->GetName());
//	//	}
//	//	else
//	//	{
//	//		UE_LOG(LogTemp, Warning, TEXT("Objective is NULL"));
//	//	}
//	//}
//
//	//GS = GetGameState<AFPSInGameState>();
//	if (GS)
//	{
//		GS->MissionFailed(NewViewTarget, SeenPawn, false);
//		UE_LOG(LogTemp, Warning, TEXT("GameState Is not NULL"));
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("GameState Is NULL"));
//	}
//}
//
//
//
///// Level Load
//void AFPSInGameMode::PostLogin(APlayerController* NewPlayer)
//{
//	Super::PostLogin(NewPlayer);
//
//	AFPSPlayerState* PS = Cast<AFPSPlayerState>(NewPlayer->PlayerState);
//
//	++NumberofPlayers;
//
//	/*auto Game = GetGame();
//	if (PS && Game)
//	{
//		Game->NewPlayer(PS);
//	}*/
//
//
//}
//
//void AFPSInGameMode::ConnectLobbyWidget(UUILobby* Lobby)
//{
//	UILobby = Lobby;
//	UILobby->Start.AddDynamic(this, &AFPSInGameMode::StartGame);
//}
//
//
//
//void AFPSInGameMode::TraveNewMap(/*FString URL*/)
//{
//
//	UWorld* World = GetWorld();
//	if (!ensure(World != nullptr)) return;
//
//	bUseSeamlessTravel = true;
//	World->ServerTravel("/Game/Maps/City?listen");
//
//	LOG_S(FString("TraveNewMap ServerTravel"));
//}
//
//
//void AFPSInGameMode::StartGame()
//{
//	UWorld* World = GetWorld();
//	if (!ensure(World != nullptr)) return;
//
//	bUseSeamlessTravel = true;
//	World->ServerTravel("/Game/Maps/FirstPersonExampleMap?listen");
//
//	// Get GameInstance
//	auto GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
//	GameInstance->StartSession();
//	LOG_S(FString("StartGame"));
//	//GetWorldTimerManager().ClearTimer(TimerHandle_StartGame);
//}
//
//
///// GameObject System
//void AFPSInGameMode::SetGame(AFPSGameObject* _Game)
//{
//	auto gs = Cast<AFPSInGameState>(GameState);
//	if (IsValid(gs))
//	{
//		gs->SetGame(_Game);
//	}
//	else
//	{
//		UE_LOG(LogTemp, Error, TEXT("GameMode::Cannot set Game: GameState is invalid!"));
//	}
//}
//
//
//AFPSGameObject* AFPSInGameMode::GetGame()
//{
//	auto gs = Cast<AFPSInGameState>(GameState);
//	if (IsValid(gs))
//	{
//		return gs->GetGame();
//	}
//	return nullptr;
//}