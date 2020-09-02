// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Public/FPSPlayerState.h"
#include "FPSCommon.h"
#include "FPSGameObject.generated.h"

//USTRUCT(BlueprintType)
//struct FPlayerData
//{
//	GENERATED_USTRUCT_BODY()
//
//	ETypeOfPawn PawnType;
//
//	FString PlayerPawnName;
//
//	int32 ScorePawn;
//
//};

UCLASS()
class FPSGAME_API AFPSGameObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSGameObject();

	UPROPERTY()
	TMap<int32, FPlayerData> PlayerStatesData;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	// BEGIN Unreal Games Game Interface 
	/**
	* Starts new game.
	*/
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "UnrealGames")
	virtual void StartGame();


	/*UFUNCTION(BlueprintCallable, Category = "InGameInstance")
	void NewPlayer(AFPSPlayerState* Player);
*/
	void UpdatePlayers(AFPSPlayerState* OldPS, AFPSPlayerState* NewPS);


	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "UnrealGames")
	void NewPlayer(AFPSPlayerState* Player);

	//FString TestName;

public:

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "UnrealGames")
	bool bIsInGame;
	
	/* PlayerState Setter Classs */
	UPROPERTY(EditDefaultsOnly, Category = "UnrealGames")
	TSubclassOf<AFPSPlayerState> PlayerStateClass;


	// Clients who are actually in game (not spectators or whatever)
	UPROPERTY(BlueprintReadOnly)
	TArray<AFPSPlayerState*> Players;

};
