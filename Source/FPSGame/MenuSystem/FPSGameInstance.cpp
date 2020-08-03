// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameInstance.h"

#include "MainMenu.h"
#include "../DebugTool/DebugLog.h"
#include "JoinMenu.h"

#include "UObject/ConstructorHelpers.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemTypes.h"
#include "UnrealNames.h"
#include "../Public/FPSPlayerController.h"


const static FName SESSION_NAME = EName::NAME_GameSession;
const static FName SESSION_NAME_SETTINGS_KEY = TEXT("Server Name");

UFPSGameInstance::UFPSGameInstance()
{

	ConstructorHelpers::FClassFinder<UMainMenu> MainMenuBPClass(TEXT("/Game/UI/MenuSystem/WBP_MainMenu"));
	if (!ensure(MainMenuBPClass.Class != nullptr)) return;

	MainMenu = MainMenuBPClass.Class;
	LOG_S(FString(GetName()));
}

void UFPSGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	SetLanOrInternet();

	if (Subsystem != nullptr)
	{
		LOG_S(FString::Printf(TEXT("OnlineSubSystem Is not Null = %s"), *Subsystem->GetSubsystemName().ToString()));
		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnCreateSessionComplite);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UFPSGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		LOG_S(FString("OnlineSubSystem = NULL"));
	}
}

void UFPSGameInstance::SetLanOrInternet()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();

	if (SessionSearch.IsValid())
	{
		if (Subsystem->GetSubsystemName() == "NULL")
		{
			SessionSettings.bIsLANMatch = true;
			SessionSearch->bIsLanQuery = true;

		}
		else
		{
			SessionSettings.bIsLANMatch = false;
			SessionSearch->bIsLanQuery = false;
		}
	}
}

void UFPSGameInstance::Host(FString UserServerName)
{
	DesiredServerName = UserServerName;

	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			this->CreateActualSession();
		}
		
	}
}


void UFPSGameInstance::CreateActualSession()
{
	if (SessionInterface.IsValid())
	{
		SessionSettings.NumPublicConnections = 3;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.Set<FString>(SESSION_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}


void UFPSGameInstance::OnCreateSessionComplite(FName SessionName, bool Success)
{
	if (!Success)
	{
		LOG_S(FString("Could not Create Session"));
		return;
	}

	UWorld *World = GetWorld();
	if (!ensure(World != nullptr)) return;
	World->ServerTravel("/Game/Maps/Lobby?listen");

	LOG_S(FString("Server is traveled"));
	LOG_S(SessionName.ToString());

	FString Name;
	SessionInterface->GetSessionSettings(SessionName)->Get<FString>(SESSION_NAME_SETTINGS_KEY, Name);
	//EOnlineSessionState ST = SessionInterface->GetSessionState(SessionName);
	FString SessionState = EOnlineSessionState::ToString(SessionInterface->GetSessionState(SessionName));
	LOG_S(FString::Printf(TEXT("SessionState = %s, HostName = %s"), *SessionState, *Name));
}


void UFPSGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
	if (Success)
	{
		//LOG_S(FString("DestroySessionDelegate_Start"));
		this->CreateActualSession();
		//LOG_S(FString("DestroySessionDelegate_End"));
	}
}

void UFPSGameInstance::RefreshServerList()
{
	if (SessionSearch.IsValid())
	{
		SessionSearch->MaxSearchResults = 50;
		SessionSearch->QuerySettings.Set<bool>(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		int32 Time = int32(GetWorld()->TimeSeconds);
		LOG_S(FString("Start Find Session"));
		LOG_I(Time);
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}


void UFPSGameInstance::OnFindSessionComplete(bool Success)
{
	


	if (Success && SessionSearch.IsValid())
	{
		TArray<FServerData> DataServers;
		LOG_S(FString::Printf(TEXT("SessionSearch Result Num = %i"), SessionSearch->SearchResults.Num()));
		for (FOnlineSessionSearchResult &SearchResult : SessionSearch->SearchResults)
		{
			FServerData DataServer;
			FString Name;

			if (SearchResult.Session.SessionSettings.Get<FString>(SESSION_NAME_SETTINGS_KEY, Name))
			{
				DataServer.Name = Name;
				LOG_S(FString::Printf(TEXT("Sesssion Name = %s"), *Name));
			}
			else
			{
				LOG_S(FString("Could not Find Server Name"));
			}

			DataServer.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			DataServer.CurrentPlayers = DataServer.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			DataServer.HostUsername = SearchResult.Session.OwningUserName;

			LOG_S(FString::Printf(TEXT("Sesssion PC = %s"), *SearchResult.Session.OwningUserName));
			LOG_S(FString::Printf(TEXT("Search Result ID = %s"), *SearchResult.GetSessionIdStr()));

			DataServers.Add(DataServer);

		}

		LOG_S(FString("Finshed Find Session"));
		int32 Time = int32(GetWorld()->TimeSeconds);
		LOG_I(Time);


		if (!ensure(JoinMenuClass != nullptr)) return;
		JoinMenuClass->SetServerList(DataServers);
		/*if (SessionSearch->SearchResults.Num() != 0)
			LOG_S(FString::Printf(TEXT("OnFindFunction GetNamedSession = %s"), *SessionInterface->GetNamedSession(SESSION_NAME)->SessionName.ToString()));*/
	}
	

}


void UFPSGameInstance::Join(uint32 ServerIndex)
{
	if (!SessionSearch.IsValid() && !SessionInterface.IsValid()) return;
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[ServerIndex]);
	LOG_S(FString("Start Join To Server"));
}

void UFPSGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	//auto PC = GetFirstLocalPlayerController();
	//PC->ClientTravel(adress, ETravelType::TRAVEL_Absolute);

	LOG_S(FString("Finshed Join To Server"));

	FString Adress;

	if (!SessionInterface.IsValid()) return;
	bool Success = SessionInterface->GetResolvedConnectString(SESSION_NAME, Adress);

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!Success && !PlayerController) return;

	MenuLaunch->RemoveFromParent();
	JoinMenuClass->SetGameMode();

	PlayerController->ClientTravel(Adress, ETravelType::TRAVEL_Absolute);
}



void UFPSGameInstance::LoadMainMenu()
{
	if (!ensure(MainMenu != nullptr)) return;
	MenuLaunch = CreateWidget<UMainMenu>(this, MainMenu);

	if (!ensure(MenuLaunch != nullptr)) return;

	MenuLaunch->SetUIScreen();

	MenuLaunch->SetFPSGameInstacne(this);

	LOG_S(FString("SetGameInstace"));

}


void UFPSGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
	{
		SessionInterface->StartSession(SESSION_NAME);
	}
}

void UFPSGameInstance::Report()
{
	//UE_LOG(LogTemp, Warning, TEXT("GameInsatce Report"));
}



