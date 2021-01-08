// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerClimb.generated.h"

namespace EDrawDebugTrace { enum Type; };

UENUM(BlueprintType)
enum class EClimbState : uint8 
{
	ClimbLine,
	ClimbCorner
};

class AFPSMannequin;
class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAME_API UFPSPlayerClimb : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSPlayerClimb();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintPure, Category = "Climb")
	inline EClimbState GetClimbState() { return ClimbSate; }

	UFUNCTION(BlueprintPure, Category = "MoveLedge")
	inline float GetInputDirection() { return PlayerInputDirection; }

private:

	bool LedgeLeftTracer();
	bool LedgeRightTracer();

	bool CornerLeftTracer();
	bool ConerRightTracer();

	void MoveLedgeLeft(float DeltaT);
	void MoveLedgeRight(float DeltaT);
	void ExitLedge();
	UFUNCTION(Server, Reliable, WithValidation)
	void SR_ExitLedge();

	UFUNCTION(Server, Reliable, WithValidation)
	void SR_SetPlayerInputDirection(float val);

private:

	/*UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ClimbCornerAnim;*/

	EDrawDebugTrace::Type DebugTracer;
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool DrawDebugTraceCapsule;

	AFPSMannequin* Player;

	/*bool CanMoveLeft;
	bool CanMoveRight;*/

	UPROPERTY(Replicated)
	bool bMoveLeft;
	UPROPERTY(Replicated)
	bool bMoveRight;

	UPROPERTY(Replicated)
	FVector CurrentV;
	UPROPERTY(Replicated)
	FVector TargetV;
	UPROPERTY(Replicated)
	FVector MoveLocation;

	UPROPERTY(Replicated)
	float PlayerInputDirection;

	EClimbState ClimbSate = EClimbState::ClimbLine;
	
};
