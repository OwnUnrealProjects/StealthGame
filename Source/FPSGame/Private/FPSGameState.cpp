// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "FPSPlayerController.h"
#include "FPSCharacter.h"
#include "FPSPlayerState.h"
#include "../DebugTool/DebugLog.h"
#include "../Game/FPSGameObject.h"
#include "../Game/FPSInGameInstance.h"
#include "FPSGameMode.h"
#include "../Game/FPSSaveGame.h"

#include "Public/EngineUtils.h"




AFPSGameState::AFPSGameState()
{
	//PrimaryActorTick.bCanEverTick = true;
}

void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();
	LOG_S(FString(" GameState"));

	AFPSGameMode* GM = GetWorld() ? Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode()) : nullptr;
	/*if (GM)
	{
		GM->SavePlayerStatesData.AddDynamic(this, &AFPSGameState::SavePlayerStatesData);
	}*/
}


void AFPSGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlayerArray.Num() != 0)
	{
		for (int32 i = 0; i < PlayerArray.Num(); i++)
		{
			LOG_S(FString(PlayerArray[i]->GetPlayerName()));
		}
	}
}


void AFPSGameState::MultiCastOnMissionComplite_Implementation(AActor *CameraNewTarget)
{

	
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		if (PC && PC->IsLocalController() && PC->GetPawn())
		{
			auto Player = Cast<AFPSCharacter>(PC->GetPawn());
			UE_LOG(LogTemp, Warning, TEXT("GameState Player name = %s,  CarryingObjective = %i"), *Player->GetName(), Player->bIsCarryingObjective);
			PC->SetViewTargetWithBlend(CameraNewTarget, 1.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			PC->OnMissionCompleted(Player->bIsCarryingObjective);
			PC->GetPawn()->DisableInput(PC);


			//TimerDel.BindUFunction(this, FName("UnPossessedPawn"), PC);
			//GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 3.f, false);
			//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFPSGameState::UnPossessedPawn, 3.f, false);
		}

	}

	/*for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* Pawn = It->Get();
		if (Pawn && Pawn->IsLocallyControlled())
		{
			Pawn->DisableInput(nullptr);
			UE_LOG(LogTemp, Warning, TEXT("MultiCastOnMissionComplite Implementation"));
		}
	}*/

	

}

void AFPSGameState::MissionFailed_Implementation(AActor * CameraNewTarget, APawn * InstigatorPawn, bool MissionFail)
{

	/*auto Player = Cast<AFPSCharacter>(InstigatorPawn);
	if (!Player) return;*/

	auto Player = InstigatorPawn;

	UE_LOG(LogTemp, Warning, TEXT("GameState MissionFailed Player Name = %s"), *InstigatorPawn->GetName());

	AFPSPlayerController* PC = Cast<AFPSPlayerController>(Player->GetController());
	PC->GetPawn()->DisableInput(PC);
	if (!PC) return;

	/*if (PC->IsLocalController())
	{
		PC->SetViewTargetWithBlend(CameraNewTarget, 1.5f, EViewTargetBlendFunction::VTBlend_Cubic);
		PC->OnMissionCompleted(false);
		PC->GetPawn()->DisableInput(PC);
		UE_LOG(LogTemp, Warning, TEXT("GameState Unpossesed Block"));
	}*/

	TimerDel.BindUFunction(this, FName("UnPossessedPawn"), PC);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 3.f, false);
	

}

void AFPSGameState::UnPossessedPawn(AFPSPlayerController *PC)
{
	UE_LOG(LogTemp, Warning, TEXT("Timer"));
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	PC->UnPossess();
}



/// ===============================================================

void AFPSGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (!World) return;


	for (TActorIterator<APlayerState> It(World); It; ++It)
	{
		AddPlayerState(*It);
	}

	/*auto GI = Cast<UFPSInGameInstance>(GetGameInstance());
	if (GI->Game->PlayerStatesData.Num() != 0)
	{

		for (int32 i = 0; i < PlayerArray.Num(); i++)
		{
			auto ValidID = GI->Game->PlayerStatesData.Find(PlayerArray[i]->PlayerId);
			if(!ValidID) continue;

			PlayerArray[i]->SetPlayerName(GI->Game->PlayerStatesData[PlayerArray[i]->PlayerId].PlayerPawnName);
			PlayerArray[i]->Score = GI->Game->PlayerStatesData[PlayerArray[i]->PlayerId].ScorePawn;
		}
	}*/
}


void AFPSGameState::AddPlayerState(APlayerState* PlayerState)
{
	if (!PlayerState->bIsInactive)
	{
		PlayerArray.AddUnique(PlayerState);
	}
}


// SaveGame Part
void AFPSGameState::SavePlayerData_Implementation(const TArray<AFPSPlayerController*> &ConnectPC)
{
	UFPSSaveGame* SaveGameInstance = UFPSSaveGame::CreateSaveGameObject();
	for (AFPSPlayerController* PC : ConnectPC)
	{
		if (PC)
		{
			FPlayerData Data;
			AFPSPlayerState* PS = Cast<AFPSPlayerState>(PC->PlayerState);
			Data.PlayerPawnName = PS->GetOwnerPlayerName();
			Data.ScorePawn = PS->GetOwnerPlayerScore();
			SaveGameInstance->PlayerStatesData.Add(PS->PlayerId, Data);
		}

	}


	//for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	//{

	//	AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
	//	//if (PC->IsLocalController())
	////	{
	//		FPlayerData Data;
	//		auto PS = Cast<AFPSPlayerState>(PC->PlayerState);
	//		Data.PlayerPawnName = PS->GetOwnerPlayerName();
	//		Data.ScorePawn = PS->GetOwnerPlayerScore();
	//		SaveGameInstance->PlayerStatesData.Add(PS->PlayerId, Data);
	////	}
	//}

	UFPSSaveGame::SaveGameObject(SaveGameInstance);
}

// GameObject part
void AFPSGameState::SavePlayerStatesData()
{

	auto GI = Cast<UFPSInGameInstance>(GetGameInstance());
	if(!GI) return;

	FPlayerData Data;
	for (int32 i = 0; i < PlayerArray.Num(); i++)
	{
		Data.PlayerPawnName = PlayerArray[i]->GetPlayerName();
		Data.ScorePawn = PlayerArray[i]->Score;
		GI->Game->PlayerStatesData.Add(PlayerArray[i]->PlayerId, Data);
	}
}
