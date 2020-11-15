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
class UFPSPlayerFireComponent;
class UTexture2D;
enum  class EFightState : uint8;

class UAnimMontage;
class AFPSStone;
class UArrowComponent;


UENUM()
enum  class EFightAnim : uint8
{
	None,
	PlayAimAnim,
	PlayUndoAimAnim,
	PlayFireAnim
};

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
	float FireDistance;

	UPROPERTY(BlueprintReadWrite, Category = "Feature")
	float AimPrecision;

	UPROPERTY(BlueprintReadWrite, Category = "Feature")
	UTexture2D* PlayerImage;

};

UCLASS()
class FPSGAME_API AFPSMannequin : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSMannequin(const FObjectInitializer& ObjectInitializer);

private:

	FString GetEnumText(ENetRole Role);

	AFPSPlayerController* OwnController;

protected:
	
	/// Pawn Consistence Part
		/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSeat")
		USceneComponent* CameraSeat;*/
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraArm")
		USpringArmComponent* CameraArm;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* CameraComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aiming")
		UArrowComponent* StoneSpawnPoint;


	/// Pawn Ability Components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		UPawnNoiseEmitterComponent* MannequinNoiseEmitterComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerInput* MannequinInputComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerAiming* MannequinAimingComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerFireComponent* MannequinFireComponent;


	/// Pawn Properties. I Set it in Blueprint
		UPROPERTY(Replicated, BlueprintReadWrite, Category = "Feature")
		FCharcterFeature OwnFeatures;



	/// Pawn Movement
		/*UPROPERTY(BlueprintReadWrite, Category = "CaracterMovement")
		float DefaultMaxSpeed;
		UPROPERTY(BlueprintReadWrite, Category = "CaracterMovement")
		float DefaultCrouchSpeed;*/
		UPROPERTY(Replicated, BlueprintReadOnly, Category = "CharacterAnim")
		bool bMoving;
		UPROPERTY(Replicated, BlueprintReadWrite, Category = "CharacterAnim")
		bool bCrouch;
		UPROPERTY(Replicated, BlueprintReadWrite, Category = "AIPerception")
		float Loudness;

	/// FIRE
		// Fire AimPoint & Fire Animation
		UPROPERTY(Replicated, EditDefaultsOnly, Category = "Gameplay")
		UAnimMontage* FightAnimMontage;
		UPROPERTY(BlueprintReadOnly, Category = "CharacterAnim")
		bool bFire;
		UPROPERTY(Replicated)
		float FireAnimPlayRate = 1.5f;

		// Fire Random Point
		UPROPERTY(Replicated, BlueprintReadOnly, Category = "CharacterAnim")
		bool bAiming;
		bool ClientRandomFireRotate;
		UPROPERTY(Replicated)
		FRotator RandomPointRotation;
	


		//Projectile
		/** Projectile class to spawn */
		UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AFPSStone> StoneBlueprinClass;
		UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		float BulletSpread;

protected:
	
/// Replication

	/// FIRE 
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void Fire();

		// RPC is also called from HostPlayer
		UFUNCTION(Server, Reliable, WithValidation)
		void SR_FightAnim(const FName& SlotName, bool bEnabelPlayerYaw);

		UFUNCTION(NetMulticast, Reliable)
		void MC_FightAnim(const FName& SlotName);

		UFUNCTION(Server, Reliable, WithValidation)
		void SR_UpdateAiming(bool val);

		UFUNCTION(Server, Reliable, WithValidation)
		void SR_Fire();

		UFUNCTION(Server,Reliable,WithValidation)
		void SR_RotateCroshairDirection(FRotator Direction);


	void SetBeginPlayParams();
	UFUNCTION(Server, Reliable, WithValidation)
	void SR_SetBeginPlayParams();



public:
	
	/// Permissions
		// Geter and Seter of Permissions
		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		bool GetPermissionCrouch() { return bCrouch; }
		void SetPermissionCrouch(bool val);

		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		bool GetPermissionMoving() { return bMoving; }
		UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
		void SetPermissionMoving(bool val) { bMoving = val; }

		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		bool GetPermissionAiming() { return bAiming; }
		UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
		void SetPermissionAiming(bool val);

		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		bool GetPermissionFire() { return bFire; }
		UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
		void SetPermissionFire(bool val) { bFire = val; }


	/// We use it in FPSPlayerInput Class
		bool IsFightAnimation() { return FightAnimMontage ? true : false; }
		
		// RPC
		UFUNCTION(Server, Reliable, WithValidation)
		void SR_Crouch(bool UpdateCrouch);

		UFUNCTION(Server, Reliable, WithValidation)
		void SR_MakeStepNoise(bool enable);

		float GetDefalutMaxSpeed() { return OwnFeatures.MaxSpeed; }
		float GetDefaultCrouchSpeed() { return OwnFeatures.CrouchSpeed; }

		void PlayFightAnim(EFightState State);

		USpringArmComponent* GetCameraArmComponent() const { return CameraArm; }

	/// We use it in FPSPlayerAiming Class
		UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	
	/// Blueprint Functions
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

	virtual void UnPossessed() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
