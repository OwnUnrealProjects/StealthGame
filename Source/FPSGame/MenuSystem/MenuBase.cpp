// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuBase.h"
#include "FPSGameInstance.h"
#include "../DebugTool/DebugLog.h"


UFPSGameInstance* UMenuBase::GameInstance = nullptr;

void UMenuBase::SetUIScreen()
{
	this->AddToViewport();
	SetUIMode();

}



void UMenuBase::SetUIMode()
{
	auto World = GetWorld();

	auto PlayeController = World->GetFirstPlayerController();

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(this->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayeController->SetInputMode(Mode);
	PlayeController->bShowMouseCursor = true;

}


void UMenuBase::SetGameMode()
{
	this->RemoveFromParent();

	auto World = GetWorld();

	auto PlayeController = World->GetFirstPlayerController();

	FInputModeGameOnly Mode;
	PlayeController->SetInputMode(Mode);
	PlayeController->bShowMouseCursor = false;
}
