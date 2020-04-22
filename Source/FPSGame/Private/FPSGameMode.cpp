// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FPSGameState.h"
#include "FPSObjectiveActor.h"

#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/TargetPoint.h"



AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	GameStateClass = AFPSGameState::StaticClass();
}



void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay"));
	//GameStateClass = AFPSGameState::StaticClass();
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
		

		AFPSGameState* GS = GetGameState<AFPSGameState>();
		if (GS)
		{
			GS->MultiCastOnMissionComplite(NewViewTarget);
			UE_LOG(LogTemp, Warning, TEXT("GameState Is not NULL"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameState Is NULL"));
		}


	

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

	AFPSGameState* GS = GetGameState<AFPSGameState>();
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