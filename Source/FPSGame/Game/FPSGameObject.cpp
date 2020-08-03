// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameObject.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AFPSGameObject::AFPSGameObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bIsInGame = true;
	//TestName = "GameObjectExist";

}

// Called when the game starts or when spawned
void AFPSGameObject::BeginPlay()
{
	Super::BeginPlay();
	
}


void AFPSGameObject::StartGame()
{
	
}

//void AFPSGameObject::NewPlayer(AFPSPlayerState* Player)
//{
//	Players.AddUnique(Player);
//}

void AFPSGameObject::UpdatePlayers(AFPSPlayerState* OldPS, AFPSPlayerState* NewPS)
{
	if (OldPS == nullptr || NewPS == nullptr)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UGGame::UpdatePlayerState: Invalid argument."));
		return;
	}

	int32 i = Players.Find(OldPS);
	if (i != INDEX_NONE)
	{
		Players.Remove(OldPS);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("UGGame::UpdatePlayerState: OldPS (%s) is not in game."), *GetNameSafe(OldPS));
		return;
	}

	Players.Add(NewPS);
}

void AFPSGameObject::NewPlayer_Implementation(AFPSPlayerState* Player)
{
	if (!bIsInGame)
	{
		Players.AddUnique(Player);
	}
}


void AFPSGameObject::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSGameObject, bIsInGame);

}
