// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuBase.h"
#include "JoinMenu.generated.h"


USTRUCT()
struct FServerData
{
	GENERATED_BODY()

	FString Name;
	uint16 CurrentPlayers;
	uint16 MaxPlayers;
	FString HostUsername;
};


/**
 * 
 */
UCLASS()
class FPSGAME_API UJoinMenu : public UMenuBase
{
	GENERATED_BODY()
	
public:

	UJoinMenu();

	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmJoinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;

	// Scroll Box
	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* ServerList;

	/*UPROPERTY(meta= (BindWidget))
	class UEditableTextBox *IPAdressField;*/


	void SetServerList(TArray<FServerData> dataservers);

	void SetSelectIndex(uint32 Index);

private:

	void UpdateSelectedServerRow();

protected:

	virtual bool Initialize() override;


	UFUNCTION()
	void JoinServer();

	UFUNCTION()
	void BackMainMenu();

private:

	TSubclassOf<class UUserWidget> ServerRow;
	
	TOptional<uint32> SelectIndex;
	
};
