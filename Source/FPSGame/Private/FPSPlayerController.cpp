// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "FPSPlayerState.h"
//#include "../Game/FPSInGameMode.h"
#include "../Public/FPSGameMode.h"
#include "../Game/FPSInGameInstance.h"
#include "../Game/FPSGameObject.h"
#include "../DebugTool/DebugLog.h"
#include "../Player/FPSMannequin.h"

AFPSPlayerController::AFPSPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	MaxAimPrecision = 10.f;
	MaxLoud = 1.f;
	MaxStrenght = 10.f;
}


void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	//OwnerPlayerRole = GetPawn()->Role;
	
	/*FString Name;
	GetName(Name);
	LOG_S(FString::Printf(TEXT("Own Name = %s   PlayerState PlayerName = %s"), *Name, *PlayerState->GetPlayerName()));*/
	LOG_S(FString("First........."));
	
	OwnCharacter = Cast<AFPSMannequin>(GetPawn());


	/*if (Role == ROLE_Authority)
		LOG_S(FString("Server Character"));
	if (Role == ROLE_AutonomousProxy)
		LOG_S(FString("Client Character"));*/

	if (!OwnCharacter)
		return;
	
	OwnCharacter->AIDetectComponent->OnComponentBeginOverlap.AddDynamic(this, &AFPSPlayerController::AIDetect);
	OwnCharacter->AIDetectComponent->OnComponentEndOverlap.AddDynamic(this, &AFPSPlayerController::AILoss);
	AIDetectRadius = OwnCharacter->AIDetectComponent->GetScaledSphereRadius();
	
}

void AFPSPlayerController::Tick(float DeltaSeconds)
{
	
	Super::Tick(DeltaSeconds);


	//if (bAIDetect)
	//{
	//	float distance = AIActorList[0].Distance;
	//	TargetAI = AIActorList[0].DetectActor;
	//	for (int32 i = 0; i < AIActorList.Num(); i++)
	//	{
	//		LOG_S(FString::Printf(TEXT("Actor - %i, Name = %s, Distance = %f"), i, *AIActorList[i].DetectActor->GetName(), AIActorList[i].Distance));
	//		if (distance < AIActorList[i].Distance)
	//		{
	//			TargetAI = AIActorList[i].DetectActor;
	//			distance = AIActorList[i].Distance;
	//		}
	//		
	//	}
	//	bAIDetect = false;
	//	LOG_S(FString("DoOnce"));
	//}

	//if (AIActorList.Num() != 0)
	//{
	//	TargetAIDistance = FVector::Distance(TargetAI->GetActorLocation(), OwnCharacter->GetActorLocation());
	//	//LOG_F(TargetAIDistance);
	//	TargetAIDistance = FMath::Clamp(TargetAIDistance, 0.f, 1000.f);
	//	GetTargetAIDistance(TargetAIDistance);
	//}


	if (AIActorList.Num() != 0)
	{
		TargetAI = AIActorList[0];
		if (AIActorList.Num() > 1)
		{
			float distance_1 = FVector::Distance(OwnCharacter->GetActorLocation(), AIActorList[0]->GetActorLocation());
			for (int32 i = 1; i < AIActorList.Num(); i++)
			{
				float distance_2 = FVector::Distance(OwnCharacter->GetActorLocation(), AIActorList[i]->GetActorLocation());
				//LOG_S(FString::Printf(TEXT("Actor - %i, Name = %s, Distance = %f"), i, *AIActorList[i]->GetName(), AIActorList[i].Distance));
				if (distance_2 < distance_1)
				{
					TargetAI = AIActorList[i];
					distance_1 = distance_2;
				}

			}

			TargetAIDistance = FVector::Distance(TargetAI->GetActorLocation(), OwnCharacter->GetActorLocation());
			//LOG_F(TargetAIDistance);
			TargetAIDistance = FMath::Clamp(TargetAIDistance, 0.f, 1000.f);
			GetTargetAIDistance(TargetAIDistance);

		}
		else
		{
			TargetAIDistance = FVector::Distance(TargetAI->GetActorLocation(), OwnCharacter->GetActorLocation());
			//LOG_F(TargetAIDistance);
			TargetAIDistance = FMath::Clamp(TargetAIDistance, 0.f, 1000.f);
			GetTargetAIDistance(TargetAIDistance);
		}
	}
}

void AFPSPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);

	OwnerPlayerRole = aPawn->Role;
	OwnerPlayerRemotRole = aPawn->GetRemoteRole();
	bPossess = true;

	LOG_S(FString("First........."));
}



//void AFPSPlayerController::AddPitchInput(float Val)
//{
//	Super::AddPitchInput(Val);
//	LOG_F(RotationInput.Pitch);
//}

void AFPSPlayerController::InitPlayerState()
{

	Super::InitPlayerState();

	LOG_S(FString("First........."));

	/*auto GI = Cast<UFPSInGameInstance>(GetGameInstance());
	auto PS = Cast<AFPSPlayerState>(PlayerState);
	if (GI && PS)
	{
		bool validArray = GI->Game->PlayerStatesData.Num() != 0 ? true : false;
		auto ValidKey = GI->Game->PlayerStatesData.Find(PS->PlayerId);

		if (ValidKey && validArray)
		{
			FString name = GI->Game->PlayerStatesData[PS->PlayerId].PlayerPawnName;
			auto score = GI->Game->PlayerStatesData[PS->PlayerId].ScorePawn;
			PlayerState->SetPlayerName(name);
			PlayerState->Score = score;
		}
		
	}*/

	//if (GetNetMode() != NM_Client)
	//{
	//	UWorld* const World = GetWorld();
	//	AFPSGameMode* const GameMode = World ? Cast<AFPSGameMode>(World->GetAuthGameMode()) : NULL;
	//	//LOG_S(FString::Printf(TEXT("GameMode Name = %s"), *GameMode->TestGameModeName));  // Crash the Editor
	//	//GameMode->TestGameModeName = "GM";
	//	//LOG_S(FString::Printf(TEXT("GameMode Name = %s"), *GameMode->TestGameModeName));
	//	if (GameMode != NULL)
	//	{
	//		FActorSpawnParameters SpawnInfo;
	//		SpawnInfo.Owner = this;
	//		SpawnInfo.Instigator = Instigator;
	//		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//		SpawnInfo.ObjectFlags |= RF_Transient;	// We never want player states to save into a map
	//		PlayerState = World->SpawnActor<APlayerState>(GameMode->GetPlayerStateClass(), SpawnInfo);
	//		

	//		// force a default player name if necessary
	//		if (PlayerState && PlayerState->GetPlayerName().IsEmpty())
	//		{
	//			// don't call SetPlayerName() as that will broadcast entry messages but the GameMode hasn't had a chance
	//			// to potentially apply a player/bot name yet
	//			PlayerState->GetPlayerName() = GameMode->DefaultPlayerName.ToString();
	//		}
	//	}
	//	else
	//	{
	//		Super::InitPlayerState();
	//	}
	//}
}

FString AFPSPlayerController::RoleString()
{
	return "None";
}



void AFPSPlayerController::OnRep_Possess()
{
	OwnerPlayerRole = ENetRole::ROLE_AutonomousProxy;
	OwnerPlayerRemotRole = ENetRole::ROLE_Authority;
}

void AFPSPlayerController::AIDetect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor)
		return;

	if(OtherActor->Tags.Num() != 0)
		LOG_S(OtherActor->GetName());


	/*float distance = FVector::Distance(OtherActor->GetActorLocation(), OwnCharacter->GetActorLocation());

	FAIData AIData;
	AIData.DetectActor = OtherActor;
	AIData.Distance = distance;

	AIActorList.Add(AIData);*/

	AIActorList.Add(OtherActor);


	LOG_I(AIActorList.Num());

	bAIDetect = true;

}


void AFPSPlayerController::AILoss(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
		return;

	if (OtherActor->Tags.Num() != 0)
		LOG_S(OtherActor->GetName());

	/*for (int32 i = 0; i < AIActorList.Num(); i++)
	{
		if (OtherActor == AIActorList[i].DetectActor)
		{
			AIActorList.RemoveAt(i);
			break;
		}
	}*/

	if(AIActorList.Num() > 0)
		AIActorList.Pop(OtherActor);

	if (AIActorList.Num() == 0)
		GetTargetAIDistance(AIDetectRadius);
	LOG_I(AIActorList.Num());

}

void AFPSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSPlayerController, bPossess);
	DOREPLIFETIME(AFPSPlayerController, CharacterName);
	DOREPLIFETIME(AFPSPlayerController, MaxAimPrecision);
	DOREPLIFETIME(AFPSPlayerController, MaxLoud);
}