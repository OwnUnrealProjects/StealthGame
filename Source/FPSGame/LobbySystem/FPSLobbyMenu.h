// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Game/FPSCommon.h"
#include "FPSLobbyMenu.generated.h"

class UInputComponent;

/**
 * 
 */
UCLASS()
class FPSGAME_API UFPSLobbyMenu : public UUserWidget
{
	GENERATED_BODY()
	
protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void OnCharacterSelect(ETypeOfPawn PawnType);

	virtual bool Initialize() override;

	UFUNCTION(BlueprintCallable, Category = "Select Pawn")
	void SelectNextCharacter();

	UFUNCTION(BlueprintCallable, Category = "Select Pawn")
	void SelectPreviousCharacter();

	ETypeOfPawn SelectPawnType();

	void SetPlayerInputComponent();

protected:

	UInputComponent* ControllerInputComonent;

public:

	UPROPERTY(meta = (BindWidget))
	class UButton* NextCharacter;

	UPROPERTY(meta = (BindWidget))
	class UButton* PreviousCharacter;


	ETypeOfPawn ChosenPawnType;
	int32 PawnTypeIndex;
	
};
