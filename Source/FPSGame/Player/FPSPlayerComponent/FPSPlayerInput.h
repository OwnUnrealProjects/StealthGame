// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerInput.generated.h"



class UInputComponent;
class AFPSMannequin;
class UFPSPlayerAiming;

UENUM()
enum  class EFireState : uint8
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

private:

	UInputComponent* InputComponentt = nullptr;
	AFPSMannequin* Player = nullptr;
	float CameraArmTargetRotation;

	EFireState FireState;

	UFPSPlayerAiming* AimingComponent;



public:

	/// Setup (assumed) attached input component
	void SetupInputComponent(AFPSMannequin* OwnerPlayer, UInputComponent* Input);

private:

	/// Function Ability
	void Azimuth(float Val);
	void Elevation(float Val);

	void MoveForward(float Val);
	void MoveRight(float Val);

	void Crouch();
	void UnCrouch();

	void StartAiming();
	void UndoAiming();
	void CanAiming(bool Val);

	void StartFire();


public:	
	// Sets default values for this component's properties
	UFPSPlayerInput();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
	
};
