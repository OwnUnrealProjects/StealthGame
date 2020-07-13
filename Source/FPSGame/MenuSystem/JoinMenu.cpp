// Fill out your copyright notice in the Description page of Project Settings.

#include "JoinMenu.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "UObject/ConstructorHelpers.h"

#include "FPSGameInstance.h"
#include "../DebugTool/DebugLog.h"
#include "ServerRow.h"
#include "Components/TextBlock.h"


UJoinMenu::UJoinMenu()
{
	ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/UI/MenuSystem/WBP_ServerRow"));
	if (!ensure(ServerRowBPClass.Class != nullptr)) return;

	//MainMenu = MainMenuBPClass.Class.DefaulObject();  // If MainMenu is class UUserWidget MainMenu = nullptr;

	ServerRow = ServerRowBPClass.Class;
}


bool UJoinMenu::Initialize()
{
	bool bSuccess = Super::Initialize();
	if (!bSuccess) return false;

	/// MainMenu Buttons
	if (!ensure(ConfirmJoinButton != nullptr)) return false;
	ConfirmJoinButton->OnClicked.AddDynamic(this, &UJoinMenu::JoinServer);

	if (!ensure(CancelButton != nullptr)) return false;
	CancelButton->OnClicked.AddDynamic(this, &UJoinMenu::BackMainMenu);

	return true;
}

void UJoinMenu::SetServerList(TArray<FServerData> dataservers)
{
	if (!ensure(ServerRow != nullptr)) return;
	if (!ensure(ServerList != nullptr)) return;

	auto World = GetWorld();
	if (!ensure(World != nullptr)) return;

	ServerList->ClearChildren();

	int32 i = 0;

	for (const auto DataServer : dataservers)
	{

		UServerRow *ServerWidget = CreateWidget<UServerRow>(World, ServerRow);
		ServerWidget->ServerName->SetText(FText::FromString(DataServer.Name));
		ServerWidget->HostUserName->SetText(FText::FromString(DataServer.HostUsername));
		FString FractionText = FString::Printf(TEXT("%d / %d"), DataServer.CurrentPlayers, DataServer.MaxPlayers);
		ServerWidget->PlayerNumber->SetText(FText::FromString(FractionText));


		ServerWidget->SetUp(this, i);
		++i;
		ServerList->AddChild(ServerWidget);

	}

	LOG_S(FString("Set Server List successful executed!!!"));
	LOG_S(FString::Printf(TEXT("Numbor of Servers = %i"), i));

}

void UJoinMenu::SetSelectIndex(uint32 Index)
{
	SelectIndex = Index;
	UpdateSelectedServerRow();
}

void UJoinMenu::UpdateSelectedServerRow()
{
	/// For Visualize on Screen
	if (!ensure(ServerList != nullptr)) return;

	for (int i = 0; i < ServerList->GetChildrenCount(); i++)
	{
		auto Server = Cast<UServerRow>(ServerList->GetChildAt(i));
		if (Server != nullptr)
			Server->IsSelect = (SelectIndex.IsSet() && SelectIndex.GetValue() == i);
	}

}


void UJoinMenu::JoinServer()
{
	if (GameInstance)
	{
		/*if (!ensure(IPAdressField != nullptr)) return;
		auto Adress = IPAdressField->GetText().ToString();
		SetGameMode();
		GameInstance->Join(Adress);
		UE_LOG(LogTemp, Warning, TEXT("Join Server"));*/
	}

	if (GameInstance && SelectIndex.IsSet())
	{
		LOG_I(SelectIndex.GetValue());
		GameInstance->Join(SelectIndex.GetValue());
	}
	
}

void UJoinMenu::BackMainMenu()
{
	this->RemoveFromParent();
}
