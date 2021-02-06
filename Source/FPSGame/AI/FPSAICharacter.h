// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAICharacter.generated.h"

class UBoxComponent;
class UPawnSensingComponent;
class USphereComponent;
class AFPSAIGuard;

UENUM(BlueprintType)
enum class EShootState : uint8
{
	None,
	HeadShoot,
	BodyShoot
};

UCLASS()
class FPSGAME_API AFPSAICharacter : public ACharacter
{
	GENERATED_BODY()
		
public:
	// Sets default values for this character's properties
	AFPSAICharacter();

	/* I don't use this functions */
	UFUNCTION(BlueprintPure, Category = "Projectile")
	bool GetHit() { return bHit; }
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetHit(bool hit) { bHit = hit; }
	/* ================================ */

	UFUNCTION(BlueprintPure, Category = "Alarm")
	inline bool GetAlarm() { return bAlarm; }
	UFUNCTION(BlueprintPure, Category = "Alarm")
	inline AActor* GetAlarmInstigator() { return AlarmInstigator; }

	UFUNCTION(BlueprintPure, Category = "Projectile")
	EShootState GetShootState() { return ShootState; }
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetShootState(EShootState state) { ShootState = state; }

	UFUNCTION(BlueprintPure, Category = "Projectile")
	float GetHitDirection() { return StoneHitDirection; }


protected:

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UBoxComponent* CatchPlayerCollision;

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	USphereComponent* HeadCollision;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UPawnSensingComponent* PawnSensingComp;

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Projectile")
	bool bHit = false;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Projectile")
	FVector HitDirection;

	/* Alarm Instigators */
	UPROPERTY(EditInstanceOnly, Category = "Friends")
	TArray<AFPSAIGuard*> FreindList;

protected:

	UFUNCTION()
	void HandlePlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPawnHearing(APawn* NInstigator, const FVector & Location, float Volume);

	UFUNCTION(BlueprintCallable, Category = "Perception")
	void TurnHearingPoint(FVector Point);

	UFUNCTION()
	void HeadShoot(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void BodyShoot(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void HandleAlarmEvent(bool Alarm, AActor* AlarmActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:

	UPROPERTY(Replicated)
	EShootState ShootState = EShootState::None;
	UPROPERTY(Replicated)
	float StoneHitDirection;

	bool bFirstAlarm = true;
	bool bAlarm;
	AActor* AlarmInstigator;
	
};
