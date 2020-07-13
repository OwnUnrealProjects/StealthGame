// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuBase.h"
#include "MainMenu.generated.h"


/**
 * 
 */
UCLASS()
class FPSGAME_API UMainMenu : public UMenuBase
{
	GENERATED_BODY()
	
	
public:

	UMainMenu();

	//void HostServer();

public:

	/// Blueprint Widgets
	/*UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* MenuSwitcher;*/

	UPROPERTY(meta = (BindWidget))
	class UWidget* MainMenu;

		// MainMenu Buttons
		UPROPERTY(meta = (BindWidget))
		class UButton* HostButton;

		UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;

		UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	TSubclassOf<class UHostMenu> HostMenu;
	class UHostMenu *HostMenuLaunch;

	TSubclassOf<class UJoinMenu> JoinMenu;
	class UJoinMenu *JoinMenuLaunch;


protected:

	virtual bool Initialize() override;

	UFUNCTION()
	void OpenHostMenu();

	UFUNCTION()
	void OpenJoinMenu();

	UFUNCTION()
	void QuitGame();


};
