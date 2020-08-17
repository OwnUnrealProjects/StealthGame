// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLobbyMenu.h"
#include "Components/Button.h"
#include "../DebugTool/DebugLog.h"




bool UFPSLobbyMenu::Initialize()
{
	bool Success = Super::Initialize();
	if(!Success) return false;

	if(!ensure(NextCharacter != nullptr)) return false;
	NextCharacter->OnClicked.AddDynamic(this, &UFPSLobbyMenu::SelectNextCharacter);

	if (!ensure(PreviousCharacter != nullptr)) return false;
	PreviousCharacter->OnClicked.AddDynamic(this, &UFPSLobbyMenu::SelectPreviousCharacter);
	
	PawnTypeIndex = 0;

	//SetPlayerInputComponent();

	return true;
}

void UFPSLobbyMenu::SelectNextCharacter()
{
	ChosenPawnType = SelectPawnType();
	OnCharacterSelect(ChosenPawnType);
}

void UFPSLobbyMenu::SelectPreviousCharacter()
{
	ChosenPawnType = SelectPawnType();
	OnCharacterSelect(ChosenPawnType);
}

ETypeOfPawn UFPSLobbyMenu::SelectPawnType()
{
	PawnTypeIndex++;
	PawnTypeIndex = PawnTypeIndex % 2;
	LOG_I(PawnTypeIndex);
	return ETypeOfPawn(PawnTypeIndex);
}

void UFPSLobbyMenu::SetPlayerInputComponent()
{
	ControllerInputComonent = GetOwningPlayer()->GetPawn()->FindComponentByClass<UInputComponent>();
	if (ControllerInputComonent)
	{
		ControllerInputComonent->BindAction("Next", IE_Pressed, this, &UFPSLobbyMenu::SelectNextCharacter);
		ControllerInputComonent->BindAction("Previous", IE_Pressed, this, &UFPSLobbyMenu::SelectPreviousCharacter);
	}
}
