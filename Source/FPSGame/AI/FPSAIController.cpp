// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIController.h"

#include "FPSAIGuard.h"


AFPSAIController::AFPSAIController()
{

}


void AFPSAIController::BeginPlay()
{
	Super::BeginPlay();

	SetOwnerPawn();

}

void AFPSAIController::SetOwnerPawn()
{
	AIPawn = Cast<AFPSAIGuard>(GetPawn());
}
