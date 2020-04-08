// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspisious,
	Alert
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UPawnSensingComponent* PawnSensingComp;

	FTimerHandle TimerHandle_ResetOrientation;

	FRotator OriginalRotation;

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;
	UFUNCTION()
	void OnRep_GuardState();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnPawnHearing(APawn* NoiseInstigator, const FVector& Location, float Volume);
	
	UFUNCTION()
	void ResetOrientation();

	void SetAIState(EAIState State);
	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);

	UFUNCTION()
	void MoveNextPatrolPoint();

	EAIState GetGuardState() { return GuardState; }

protected:

	/* Let the guard go on patrol */
	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool bPatrol;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	TArray<AActor*> PatrolPoints;

	// The Current point the actor is either moving to or standing at
	AActor* CurrentPatrolPoint;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	FString GetEnumText(ENetRole role);
	FString GetStateText(EAIState state);

private:

	int32 PatrolIndex = 0;

	AController* GuardController = nullptr;

	bool bCanTick = true;

	// for not calling multiple times CompleteMission() function 
	// because it executed Dead Animation
	bool bIsGameOver = false;

};
