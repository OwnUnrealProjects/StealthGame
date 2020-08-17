// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLobbyGameMode.h"
#include "../Public/FPSPlayerState.h"
#include "../Public/FPSPlayerController.h"
#include "../MenuSystem/FPSGameInstance.h"
#include "../DebugTool/DebugLog.h"

#include "UObject/ConstructorHelpers.h"



AFPSLobbyGameMode::AFPSLobbyGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_Player"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass_Kaya(TEXT("/Game/Player_Character/Kaya/BP_Kaya"));
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass_Aj(TEXT("/Game/Player_Character/Aj/BP_Aj"));

	if(PlayerPawnBPClass.Class != NULL)
		DefaultPawnClass = PlayerPawnBPClass.Class;

	if(PlayerPawnBPClass_Aj.Class != NULL)
		PawnType.Add(ETypeOfPawn::Aj, (UClass*)PlayerPawnBPClass_Aj.Class);

	if(PlayerPawnBPClass_Kaya.Class != NULL)
		PawnType.Add(ETypeOfPawn::Kaya, (UClass*)PlayerPawnBPClass_Kaya.Class);

}

UClass* AFPSLobbyGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{

	UWorld* MyWorld = GetWorld();
	if (MyWorld == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No World! - Spawning the default pawn !!!"));
		return DefaultPawnClass;
	}

	FString CurrentMapName = MyWorld->RemovePIEPrefix(MyWorld->GetMapName());
	if (CurrentMapName.Equals(TEXT("Lobby"), ESearchCase::IgnoreCase))
	{
		LOG_S(FString("Level is Lobby"));
		return DefaultPawnClass;
	}

	if (InController == NULL)
	{
		LOG_S(FString("InController is NULL !!!"));
		return DefaultPawnClass;
	}

	AFPSPlayerController* PC = Cast<AFPSPlayerController>(InController);
	if (PC)
	{
		LOG_S(FString("PlayerController Found"));
		AFPSPlayerState* PS = Cast<AFPSPlayerState>(PC->PlayerState);
		UClass* PawnClass = PawnType[PS->CurrentPlayerData.PawnType];

		if(PawnClass)
		{
			LOG_S(FString("PawnClass find and set Seccessfully"));
			return PawnClass;
		}
		else
		{
			LOG_S(FString("PawnClass is not find !!!"));
			return DefaultPawnClass;
		}

	}
	else
	{
		LOG_S(FString("PlayerController not Cast !!!"));
		return DefaultPawnClass;
	}

}


void AFPSLobbyGameMode::StartGamee()
{
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;

	//SavePlayerStatesData.Broadcast();
	//GS->SavePlayerData(ConnectPlayerControllers);

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/Maps/FirstPersonExampleMap?listen");

	// Get GameInstance
	auto GameInstance = Cast<UFPSGameInstance>(GetGameInstance());
	GameInstance->StartSession();

	//GetWorldTimerManager().ClearTimer(TimerHandle_StartGame);
}