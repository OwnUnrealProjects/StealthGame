// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "FPSGameState.h"
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

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	if (InstigatorPawn)
	{
		/// if check DisableInput() with buttons Alt + G two times
		/// we can see it's parameter may be PlayerController or NULL
		//InstigatorPawn->DisableInput(nullptr); // we put it in GameState class
		UE_LOG(LogTemp, Warning, TEXT("GameMode CompleteMission"))
		TArray<AActor*> BP_ClassArray;
		UGameplayStatics::GetAllActorsOfClass(this, BlueprintClass, BP_ClassArray);


		if (BP_ClassArray.Num() > 0)
		{
			NewViewTarget = BP_ClassArray[0];
			//UE_LOG(LogTemp, Warning, TEXT("BP_BlueprintArray Number = %i"), BP_ClassArray.Num());

		}

		/// TSubClassOf<>() can not access the class which is created already
		/// in this cases CameraLocatoin must be created in the world for get him Location()
		   /// but instead of CameraLocation we can use FVector & FRotator directly
		//NewViewTarget = GetWorld()->SpawnActor<AActor>(BlueprintClass);
		////CameraLocation = GetWorld()->SpawnActor<ATargetPoint>(Location);
		//NewViewTarget->SetActorLocation(FVector(-1250.000000, 680.000000, 1270.000000));
		//NewViewTarget->SetActorRotation(FRotator(-27.199799, -32.999878, 0.000006));
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			APlayerController* PC = It->Get();
			if (PC && NewViewTarget)
			{
				//UE_LOG(LogTemp, Warning, TEXT("TargetLocation = %s"), *CameraLocation->GetActorLocation().ToString());
				PC->SetViewTargetWithBlend(NewViewTarget, 1.5f, EViewTargetBlendFunction::VTBlend_Cubic);
			}
		}

		//APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
		//if (PC && NewViewTarget)
		//{
		//	//UE_LOG(LogTemp, Warning, TEXT("TargetLocation = %s"), *CameraLocation->GetActorLocation().ToString());
		//	PC->SetViewTargetWithBlend(NewViewTarget, 1.5f, EViewTargetBlendFunction::VTBlend_Cubic);
		//}
	}

	AFPSGameState* GS = GetGameState<AFPSGameState>();
	if (GS)
	{
		GS->MultiCastOnMissionComplite(InstigatorPawn, bMissionSuccess);
		UE_LOG(LogTemp, Warning, TEXT("GameState Is not NULL"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameState Is NULL"));
	}

	OnMissionCompleted(InstigatorPawn, bMissionSuccess);

}


