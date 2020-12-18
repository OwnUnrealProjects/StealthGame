// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerJump.generated.h"

class AFPSMannequin;
class UAnimMontage;

UENUM()
enum  class EJumpState : uint8
{
	None,
	Jump,
	Climbing
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAME_API UFPSPlayerJump : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSPlayerJump();

	void JumpUp();
	void JumpDown();

protected:

	void SetJumpMode(bool val);
	UFUNCTION(Server, Reliable, WithValidation)
	void SR_Jump(bool val);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:

	void JumpOnLedge();
	void HangOnLedge();
	void ClimbOnLedge();
	void ClimbFinished();

	UFUNCTION()
	void MoveToTargetFinished();

	EJumpState CalculateJumpState();

	int32 GetNextUUID() { return NextUUID++; }

	FVector CalculateTargetLocation();
	FRotator CalculateTargetRotation();

private:

	AFPSMannequin* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* JumpAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ClimbAnim;


	bool IsCharacterJump;
	bool IsHangOn = false;

	UPROPERTY(EditAnywhere, Category = "Debug")
	bool DrawDebugJumpLine;


	EJumpState JumpState = EJumpState::None;

	float MaxJumpLength = 200.f;
	float LedgeLength;
	FHitResult HitLedge;
	FVector WallLocation;
	FVector WallNormal;

	FName Socket = FName("PelvisSokect");

	int32 NextUUID = 0;

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle_Climb;
	
};
