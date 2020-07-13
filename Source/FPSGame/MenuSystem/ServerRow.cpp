// Fill out your copyright notice in the Description page of Project Settings.

#include "ServerRow.h"

#include "JoinMenu.h"

#include "Components/Button.h"


void UServerRow::SetUp(UJoinMenu *Owner, uint32 Index)
{
	OwnerClass = Owner;
	this->Index = Index;
	RowButton->OnClicked.AddDynamic(this, &UServerRow::ClickedEvent);
}


void UServerRow::ClickedEvent()
{
	OwnerClass->SetSelectIndex(this->Index);
}

