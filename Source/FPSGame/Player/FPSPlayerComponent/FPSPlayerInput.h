// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerInput.generated.h"



class UInputComponent;
class AFPSMannequin;
class UFPSPlayerAiming;

UENUM()
enum  class EFightState : uint8
{
	None,
	Aim,
	UndoAim,
	Fire
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAME_API UFPSPlayerInput : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UFPSPlayerInput();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UInputComponent* InputComponentt = nullptr;
	AFPSMannequin* Player = nullptr;
	float CameraArmTargetRotation;

	EFightState FightState;

	UFPSPlayerAiming* AimingComponent;



public:

	/// Setup (assumed) attached input component
	void SetupInputComponent(UInputComponent* Input);

private:

	/// Function Ability

	// Mouse Coursor
	void Azimuth(float Val);
	void Elevation(float Val);


	// Moving
	void MoveForward(float Val);
	void MoveRight(float Val);
	void Crouch();
	void UnCrouch();

	// Aiming
	void StartAiming();
	void ApplyAimState();

	void UndoAiming();
	void ApplyUndoAimState();

	void StartFire();
	void ApplyFireState();
	
	void ApplyFightState(EFightState State);

};
