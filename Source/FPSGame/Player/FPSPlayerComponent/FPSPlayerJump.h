// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerJump.generated.h"

class AFPSMannequin;
class UAnimMontage;
class AFPSPlayerState;

UENUM()
enum  class EJumpState : uint8
{
	None,
	HangOn,
	ClimbOn
};


USTRUCT()
struct FJumpData
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
		FVector_NetQuantize TargetLocation;
	UPROPERTY()
		FRotator TargetRotation;
};


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
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
	void SR_Jump();
	UFUNCTION(NetMulticast, Reliable)
	void MC_Jump();
	UFUNCTION(Server, Reliable, WithValidation)
	void SR_JumpDown();
	UFUNCTION(NetMulticast, Reliable)
	void MC_JumpDown();


	void HangOnLedge();
	UFUNCTION()
	void HangFinished();
	void ClimbOnLedge();
	void ClimbFinished();
	

	UFUNCTION(NetMulticast, Reliable)
	void MC_HangOnLedge();

	UFUNCTION(NetMulticast, Reliable)
	void MC_ClimbOnLedge();

	UFUNCTION(Server, Reliable, WithValidation)
	void SR_ChackLedgeHeight();



protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:


	void ForwardTracer();

	int32 GetNextUUID() { return NextUUID++; }

	FVector CalculateHangOnLocation();
	FRotator CalculateHangOnRotation();

	bool IsPlayerChangePosition();
	void setJumpData(EJumpState jumpstate);

private:

	AFPSMannequin* Player;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* JumpAnim;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ClimbAnim;



	UPROPERTY(EditAnywhere, Category = "Debug")
	bool DrawDebugJumpLine;




	float MaxJumpLength = 200.f;
	float InnerTraceDeep = 50;
	
	
	UPROPERTY(Replicated)
	bool IsHeigthLedge = false;
	UPROPERTY(Replicated)
	bool IsCharacterJump;
	UPROPERTY(Replicated)
	FHitResult HitLedgeUp;
	UPROPERTY(Replicated)
	FJumpData JumpData;



	float LedgeLength;
	UPROPERTY(Replicated)
	FVector WallLocation;
	UPROPERTY(Replicated)
	FVector WallNormal;




	FName Socket = FName("PelvisSocket");

	int32 NextUUID = 0;

	FTimerDelegate TimerDel;
	FTimerHandle TimerHandle_Climb;

};
