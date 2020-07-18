// Fill out your copyright notice in the Description page of Project Settings.

#include "HostMenu.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"

#include "FPSGameInstance.h"
#include "../Public/FPSPlayerController.h"
#include "../Public/FPSPlayerState.h"
#include "../DebugTool/DebugLog.h"



bool UHostMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	/// MainMenu Buttons
	if (!ensure(ConfirmHostButton != nullptr)) return false;
	ConfirmHostButton->OnClicked.AddDynamic(this, &UHostMenu::HostServer);

	if (!ensure(CancelButton != nullptr)) return false;
	CancelButton->OnClicked.AddDynamic(this, &UHostMenu::BackMainMenu);

	return true;
}

void UHostMenu::HostServer()
{
	
	if(!ReadyToHost()) return;

	
	if (GameInstance)
	{
		LOG_S(FString::Printf(TEXT("GameInstance Is not Null = %s"), *GameInstance->GetName()));
		SetGameMode();
		FString servername = ServerName->GetText().ToString();
		GameInstance->Host(servername);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Host Server GameInstance = NULL"));
	}

	
}

void UHostMenu::BackMainMenu()
{
	this->RemoveFromParent();
}

bool UHostMenu::ReadyToHost()
{
	if (!ensure(ServerName != nullptr)) return false;
	if (ServerName->GetText().ToString().IsEmpty()) return false;
	if (!ensure(PlayerName != nullptr)) return false;
	if (PlayerName->GetText().ToString().IsEmpty()) return false;

	return true;
}
