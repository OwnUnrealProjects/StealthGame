// Fill out your copyright notice in the Description page of Project Settings.

#include "MainMenu.h"

#include "Components/Button.h"
#include "UObject/ConstructorHelpers.h"

#include "HostMenu.h"
#include "JoinMenu.h"
#include "../DebugTool/DebugLog.h"
#include "FPSGameInstance.h"

/// Overlap macro
//#undef LOG_S
//#define LOG_S(x)


UMainMenu::UMainMenu()
{
	ConstructorHelpers::FClassFinder<UHostMenu> HostMenuBPClass(TEXT("/Game/UI/MenuSystem/WBP_HostMenu"));
	if (!ensure(HostMenuBPClass.Class != nullptr)) return;

	HostMenu = HostMenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UJoinMenu> JoinMenuBPClass(TEXT("/Game/UI/MenuSystem/WBP_JoinMenu"));
	if (!ensure(JoinMenuBPClass.Class != nullptr)) return;

	JoinMenu = JoinMenuBPClass.Class;
}


bool UMainMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	if(!bSuccess) return false;

	/// MainMenu Buttons
	if (!ensure(HostButton != nullptr)) return false;
	HostButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

	if (!ensure(JoinButton != nullptr)) return false;
	JoinButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);

	if (!ensure(QuitButton != nullptr)) return false;
	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	return true;
}

void UMainMenu::OpenHostMenu()
{

	auto World = GetWorld();
	if (!ensure(World != nullptr)) return;

	if (!ensure(HostMenu != nullptr)) return;
	HostMenuLaunch = CreateWidget<UHostMenu>(World, HostMenu);

	LOG_S(FString("OpenHostMenu"));

	HostMenuLaunch->SetUIScreen();
	LOG_S(FString("HostMenuExist"));
	

	//auto hostmenu = HostMenu->GetClass();
	//MenuSwitcher->SetActiveWidget(hostmenu);

	

}

void UMainMenu::OpenJoinMenu()
{
	auto World = GetWorld();
	if (!ensure(World != nullptr)) return;

	if (!ensure(JoinMenu != nullptr)) return;
	JoinMenuLaunch = CreateWidget<UJoinMenu>(World, JoinMenu);

	JoinMenuLaunch->SetUIScreen();

	if (GameInstance)
	{
		GameInstance->SetJoinMenuClass(JoinMenuLaunch);
		GameInstance->RefreshServerList();
	}
	
	//UE_LOG(LogTemp, Warning, TEXT("OpenJoinMenu"));
}

void UMainMenu::QuitGame()
{
	auto World = GetWorld();
	if (!ensure(World != nullptr)) return;

	auto PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;

	PlayerController->ConsoleCommand("quit");

}
