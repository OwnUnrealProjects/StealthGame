// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSInGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FPSInGameState.h"
#include "FPSObjectiveActor.h"

#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TargetPoint.h"
#include "FPSPlayerController.h"
#include "../UI/UILobby.h"
#include "../MenuSystem/FPSGameInstance.h"
#include "FPSPlayerState.h"
#include "../Game/FPSInGameInstance.h"
#include "../DebugTool/DebugLog.h"


AFPSInGameMode::AFPSInGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
	GameStateClass = AFPSInGameState::StaticClass();
	PlayerStateClass = AFPSPlayerState::StaticClass();
	PlayerControllerClass = AFPSPlayerController::StaticClass();
}



void AFPSInGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay Set GS"));

	GS = GetGameState<AFPSInGameState>();

	GS->SetPlayerNumbers(NumberofPlayers);


}



void AFPSInGameMode::CompleteMission()
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


	//AFPSInGameState* GS = GetGameState<AFPSInGameState>();
	if (GS)
	{
		GS->MultiCastOnMissionComplite(NewViewTarget);
		UE_LOG(LogTemp, Warning, TEXT("GameState Is not NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState Is NULL"));
	}

	GetWorldTimerManager().SetTimer(TimerHandle_LoadMap, this, &AFPSInGameMode::TraveNewMap, 4.f);


	//OnMissionCompleted(InstigatorPawn, bMissionSuccess);

}


void AFPSInGameMode::MissionFaild(APawn * SeenPawn)
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

	//GS = GetGameState<AFPSInGameState>();
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



/// Level Load

void AFPSInGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	//++NumberofPlayers;
	//if (GS)
	//{
	//	GS->SetPlayerNumbers(NumberofPlayers);
	//	UE_LOG(LogTemp, Warning, TEXT("GS is not NULL"));
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("GS is NULL"));
	//}
	//	

	//UE_LOG(LogTemp, Warning, TEXT("PostLogin NumberPlayers = %i"), NumberofPlayers);
	//FString NumberString = FString::FromInt(NumberofPlayers);
	//FText NumberText = FText::FromString(NumberString);
	////UILobby->PlayerNumber->SetText(NumberText);

	//if (NumberofPlayers >= 2)
	//{
	//	GetWorldTimerManager().SetTimer(TimerHandle_StartGame, this, &AFPSInGameMode::StartGame, 10.f);
	//}

	AFPSPlayerState* PS = Cast<AFPSPlayerState>(NewPlayer->PlayerState);
	UFPSInGameInstance* GI = Cast<UFPSInGameInstance>(GetGameInstance());
	if (PS && GI)
	{
		GI->NewPlayer(PS);
		GI->PlayerNumber++;
	}

}

void AFPSInGameMode::ConnectLobbyWidget(UUILobby* Lobby)
{
	UILobby = Lobby;
	UILobby->Start.AddDynamic(this, &AFPSInGameMode::StartGame);
}

void AFPSInGameMode::StartGame()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/FirstPersonExampleMap?listen");

	// Get GameInstance
	auto GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	GameInstance->StartSession();

	//GetWorldTimerManager().ClearTimer(TimerHandle_StartGame);
}


void AFPSInGameMode::TraveNewMap(/*FString URL*/)
{

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/City?listen");

	LOG_S(FString("TraveNewMap ServerTravel"));
}


