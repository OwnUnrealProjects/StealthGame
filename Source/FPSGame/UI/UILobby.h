// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "UILobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStart);

/**
 * 
 */
UCLASS()
class FPSGAME_API UUILobby : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNumber;
	

protected:

	virtual bool Initialize() override;

	void LaunchGame();


public:

	FStart Start;
	
};
