// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerFireComponent.generated.h"

class AFPSMannequin;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAME_API UFPSPlayerFireComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSPlayerFireComponent();

private:

	AFPSMannequin* Player;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	FVector GetFireDirection(FVector start, FVector end, float speed);
	bool SuggestStoneVelocity(FVector& OutTossVelocity, FVector Start, FVector End, float TossSpeed, bool bFavorHighArc);

	FVector CalculateFireDirection(FVector start, FVector end, float speed);
	
};
