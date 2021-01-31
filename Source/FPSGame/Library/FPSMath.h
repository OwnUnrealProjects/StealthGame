// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

namespace FPSMath {

	static float GetHitPointDirection(FVector HitPoint, FVector CenterPoint, AActor* Actor);

	static float GetHitPointDirectionFromForwardVector(FVector HitPoint, FVector CenterPoint, AActor* Actor);

	static float GetHitPointDirectionFromRightVector(FVector HitPoint, FVector CenterPoint, AActor* Actor);
};

