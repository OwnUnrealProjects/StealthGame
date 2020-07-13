// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "FPSGameInstance.generated.h"



/**
 * 
 */
UCLASS()
class FPSGAME_API UFPSGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFPSGameInstance();

	virtual void Init() override;

	UFUNCTION(Exec)
	void Report();
	

	/// For MainMenu Widget Functions
	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(Exec)
	void Host(FString UserServerName);

	UFUNCTION(Exec)
	void Join(uint32 ServerIndex);

	void RefreshServerList();

	void SetJoinMenuClass(class UJoinMenu *joinMenuclass) { JoinMenuClass = joinMenuclass; }

	UFUNCTION(BlueprintCallable, Category = "Game Launch")
	void StartSession();


protected:

	/// Delegate Call-Back function
	void CreateActualSession();
	void OnCreateSessionComplite(FName SessionName, bool Success);
	void OnDestroySessionComplete(FName SessionName, bool Success);
	void OnFindSessionComplete(bool Success);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	void SetLanOrInternet();

protected:

	TSubclassOf<class UMainMenu> MainMenu;
	class UMainMenu* MenuLaunch = nullptr;

	//const FName SubsystemName = "OSS";

	/// Network

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	FOnlineSessionSettings SessionSettings;

	FString DesiredServerName;

	class UJoinMenu *JoinMenuClass;

	
};
