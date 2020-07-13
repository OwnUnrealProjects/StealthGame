// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuSystem/MenuBase.h"
#include "HostMenu.generated.h"

/**
 * 
 */
UCLASS()
class FPSGAME_API UHostMenu : public UMenuBase
{
	GENERATED_BODY()
	

public:

	
	UPROPERTY(meta = (BindWidget))
	class UButton* ConfirmHostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* CancelButton;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* ServerName;
	
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* PlayerName;


protected:

	virtual bool Initialize() override;

	UFUNCTION()
	void HostServer();
	
	UFUNCTION()
	void BackMainMenu();

private:

	bool ReadyToHost();

};
