// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSPlayerComponent/FPSPlayerFireComponent.h"
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
class UFPSPlayerJump;
class UFPSPlayerClimb;
class UTexture2D;
class USphereComponent;

enum  class EFightState : uint8;
class UAnimMontage;
class AFPSStone;
class UArrowComponent;
class AFPSObjectiveActor;


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
	float Strength;

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

	TMap<float, FName> TM_ShotDirection;

private:

	/// HeadShot
		UFUNCTION()
		void GetUp();
		FTimerDelegate TimerDel;
		FTimerHandle FTimer_HeadShotAnim;

protected:
	
	/// Pawn Consistence Part
		/*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraSeat")
		USceneComponent* CameraSeat;*/
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CameraArm")
		USpringArmComponent* CameraArm;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		UCameraComponent* CameraComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aiming")
		UArrowComponent* LeftArrowLedge;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Aiming")
		UArrowComponent* RightArrowLedge;
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		UCapsuleComponent* HeadCollision;

public:

		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Collision")
		USphereComponent* AIDetectComponent;

protected:
	/// Pawn Ability Components
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
		UPawnNoiseEmitterComponent* MannequinNoiseEmitterComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerInput* MannequinInputComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerAiming* MannequinAimingComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerFireComponent* MannequinFireComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerJump* MannequinJumpComponent;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
		UFPSPlayerClimb* MannequinClimbComponent;


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
		/** Aiming Precision */
		UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
		float StoneSpread;


	/// Head Shot
		UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HeadShot")
		bool bIsShot;

		UPROPERTY(ReplicatedUsing = OnRep_ClientShot)
		FName AnimSlotname;
		UFUNCTION()
		void OnRep_ClientShot();

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HeadShot")
		float HeadShotDirection;
		/** Stand up time after Headshot */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HeadShot")
		float Solidity;

		UPROPERTY(Replicated, EditDefaultsOnly, Category = "HeadShot")
		UAnimMontage* ShotAnim;

public:

	/// Jump and Climb
		UPROPERTY(Replicated, BlueprintReadOnly, Category = "ClimbAnim")
		bool bHangOnLedge = false;
		UPROPERTY(Replicated, BlueprintReadOnly, Category = "ClimbAnim")
		float InForward;
		UPROPERTY(Replicated, BlueprintReadOnly, Category = "ClimbAnim")
		float InRight;
		/** I need it: When exit ledge not calculate LineTrace */
		UPROPERTY(Replicated)
		bool IsExitLedge = false;

		UFUNCTION(Server, Reliable, WithValidation)
		void SR_SetInRight(float val);
		UFUNCTION(Server, Reliable, WithValidation)
		void SR_SetInForward(float val);

	/// Winner Attributes
		/* Winner Flag */
		UPROPERTY(Replicated, BlueprintReadOnly, Category = "GamePlay")
		bool bIsCarryingObjective;

		/* Check if Player must be return winner Flag */
		UPROPERTY(Replicated)
		bool bReturnObjective = false;

		UPROPERTY(EditDefaultsOnly, Category = "Spectating")
		TSubclassOf<AFPSObjectiveActor> BPObjective;

	/// AI Seen
		/* Close the gate to call "Game Over" Widget in Multiple times */
		UPROPERTY(Replicated)
		bool bGuardSeen = true;


public:
	
/// RPC

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


		UFUNCTION(Server, Reliable, WithValidation)
		void SR_Crouch(bool UpdateCrouch);

		UFUNCTION(Server, Reliable, WithValidation)
		void SR_MakeStepNoise(bool enable);

protected:

	void SetBeginPlayParams();
	UFUNCTION(Server, Reliable, WithValidation)
	void SR_SetBeginPlayParams();

	/// Shoot
		// HeadShot
		UFUNCTION()
		void HeadShoot(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

		// BodyShot
		UFUNCTION()
		void HitBody(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

public:
	
	/// Permissions
		// Geter and Seter of Permissions
		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		inline bool GetPermissionCrouch() { return bCrouch; }
		void SetCrouchMode(bool val);

		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		inline bool GetPermissionMoving() { return bMoving; }
		UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
		void SetPermissionMoving(bool val) { bMoving = val; }
	
	// Really I don't use it
		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		inline bool GetPermissionAiming() { return bAiming; }
		UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
		void SetPermissionAiming(bool val);
	//  // // //

		UFUNCTION(BlueprintPure, Category = "CharacterAnim")
		inline bool GetPermissionFire() { return bFire; }
		UFUNCTION(BlueprintCallable, Category = "CharacterAnim")
		void SetPermissionFire(bool val) { bFire = val; }

		inline void SetClientRandomFireRotation(bool val) { ClientRandomFireRotate = val; }
		inline float GetFireAnimPlayRate() { return FireAnimPlayRate; }
		inline UArrowComponent* GetLeftArrowComponent() { return LeftArrowLedge; }
		inline UArrowComponent* GetRightArrowComponent() { return RightArrowLedge; }
		inline UFPSPlayerAiming* GetAimingComponent() { return MannequinAimingComponent; }

		UFUNCTION(BlueprintCallable, Category = "Climb")
		inline UFPSPlayerClimb* GetClimbingComponent() { return MannequinClimbComponent; }

		inline float GetBulletSpread() { return StoneSpread; }

	/// HeadShot
		UFUNCTION(BlueprintPure, Category = "HeadShot")
		inline bool GetShotPermission() { return bIsShot; }
		UFUNCTION(BlueprintCallable, Category = "HeadShot")
		void SetShotPermisssion(bool val) { bIsShot = val; AnimSlotname = FName("None"); }
		UFUNCTION(BlueprintPure, Category = "HeadShot")
		inline float GetHeadShotDirection() { return HeadShotDirection; }


	/// We use it in FPSPlayerInput Class // Really I don't use it
		bool IsFightAnimation() { return MannequinFireComponent->GetFightAnim() ? true : false; }
		

		inline float GetDefalutMaxSpeed() { return OwnFeatures.MaxSpeed; }
		inline float GetDefaultCrouchSpeed() { return OwnFeatures.CrouchSpeed; }

		void PlayFightAnim(EFightState State);

		inline USpringArmComponent* GetCameraArmComponent() const { return CameraArm; }

	/// We use it in FPSPlayerAiming Class
		inline UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	
	/// Blueprint Functions
		UFUNCTION(BlueprintPure, Category = "Feature")
		inline FCharcterFeature GetOwnFeature() { return OwnFeatures; }

		UFUNCTION(BlueprintImplementableEvent, Category = "ChracterAnim")
		void CanCrouched(bool Enable);

	/// Jump
		UPROPERTY(Replicated, BlueprintReadOnly, Category = "Jump")
		bool bJump;
		void JumpEvent();

		float GetPlayerHeight();
		float GetDistanceToLedge();

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

	virtual void Landed(const FHitResult& Hit) override;
	UPROPERTY(ReplicatedUSing = OnRep_Landed)
	bool IsLanded = false;
	UFUNCTION()
	void OnRep_Landed();


};




// ================================================================= //
//                 Remove TO FireComponent

/** Projectile class to spawn */
//UPROPERTY(EditDefaultsOnly, Category = "Projectile")
//TSubclassOf<AFPSStone> StoneBlueprinClass;
//
//UPROPERTY(Replicated, EditDefaultsOnly, Category = "Gameplay")
//UAnimMontage* FightAnimMontage;