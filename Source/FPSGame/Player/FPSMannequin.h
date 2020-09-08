// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSMannequin.generated.h"

class AFPSPlayerController;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AFPSProjectile;
class USoundBase;
class UAnimSequence;
class UPawnNoiseEmitterComponent;
class USpringArmComponent;
class UFPSPlayerInput;
class UFPSPlayerAiming;
class UTexture2D;
enum  class EFightState : uint8;

class UAnimMontage;


USTRUCT(BlueprintType, Blueprintable)
struct FCharcterFeature
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(BlueprintReadWrite, Category = "Feature")
	float MaxSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Feature")
	float CrouchSpeed;

	UPROPERTY(BlueprintReadWrite, Category = "Feature")
	float Smart;

	UPROPERTY(BlueprintReadWrite, Category = "Feature")
	UTexture2D* PlayerImage;

};

UCLASS()
class FPSGAME_API AFPSMannequin : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSMannequin();

private:

	FString GetEnumText(ENetRole Role);

protected:
	
	/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSeat")
	USceneComponent* CameraSeat;*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraArm")
	USpringArmComponent* CameraArm;


	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPawnNoiseEmitterComponent* NoiseEmitterComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UFPSPlayerInput* MannequinInputComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	UFPSPlayerAiming* MannequinAimingComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Feature")
	FCharcterFeature OwnFeatures;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimMontage* FireAnimation;


	UPROPERTY(BlueprintReadWrite, Category = "CaracterMovement")
	float DefaultMaxSpeed;
	UPROPERTY(BlueprintReadWrite, Category = "CaracterMovement")
	float DefaultCrouchSpeed;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "CharacterAnim")
	bool bMoving;
	UPROPERTY(Replicated, BlueprintReadWrite, Category = "CharacterAnim")
	bool bCrouch;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterAnim")
	bool bAiming;
	UPROPERTY(BlueprintReadOnly, Category = "CharacterAnim")
	bool bFire;


public:
	
	UFUNCTION(BlueprintPure, Category = "CharacterAnim")
	bool GetPermissionCrouch() { return bCrouch; }
	void SetPermissionCrouch(bool val) { bCrouch = val; }

	UFUNCTION(BlueprintPure, Category = "CharacterAnim")
	bool GetPermissionMoving() { return bMoving; }
	UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
	void SetPermissionMoving(bool val) { bMoving = val; }

	UFUNCTION(BlueprintPure, Category = "CharacterAnim")
	bool GetPermissionAiming() { return bAiming; }
	UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
	void SetPermissionAiming(bool val) { bAiming = val; }

	UFUNCTION(BlueprintPure, Category = "CharacterAnim")
	bool GetPermissionFire() { return bFire; }
	UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
	void SetPermissionFire(bool val) { bFire = val; }


	UFUNCTION(Server, Reliable, WithValidation)
	void ServerCrouch(bool UpdateCrouch);

	float GetDefalutMaxSpeed() { return DefaultMaxSpeed; }
	float GetDefaultCrouchSpeed() { return DefaultCrouchSpeed; }

	void PlayFightAnim(EFightState State);


	void Fire();

public:
	
	//USceneComponent* GetCameraSeatComponent() const { return CameraSeat; }
	USpringArmComponent* GetCameraArmComponent() const { return CameraArm; }
	UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	UFUNCTION(BlueprintPure, Category = "Feature")
	FCharcterFeature GetOwnFeature() { return OwnFeatures; }

	UFUNCTION(BlueprintImplementableEvent, Category = "ChracterAnim")
	void CanCrouched(bool Enable);

	AFPSPlayerController* GetSelfController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
