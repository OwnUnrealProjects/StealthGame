// Fill out your copyright notice in the Description page of Project Settings.

#include "UILobby.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"




bool UUILobby::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	/// MainMenu Buttons
	/*if (!ensure(StartButton != nullptr)) return false;
	StartButton->OnClicked.AddDynamic(this, &UUILobby::LaunchGame);*/

	return true;
}

void UUILobby::LaunchGame()
{
	Start.Broadcast();
}
