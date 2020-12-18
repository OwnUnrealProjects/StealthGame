// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#define DEBUGING 1
#define DEBUGING_ENEBALE(x) DEBUGING x

#if DEBUGING

#define  LOG_S(x) UE_LOG(LogTemp, Warning, TEXT("[ Function -- %s ]  String = %s"), *FString(__FUNCTION__), *x)
#define  LOG_I(x) UE_LOG(LogTemp, Warning, TEXT("[ Funtion  -- %s ]  Integer = %i"),*FString(__FUNCTION__), x)
#define  LOG_F(x) UE_LOG(LogTemp, Warning, TEXT("[ Funtion  -- %s ]  Float = %f"),*FString(__FUNCTION__), x)

#else

#define  LOG_S(x)
#define  LOG_I(x)

#endif
