// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAICharacter.generated.h"

class UBoxComponent;
class UPawnSensingComponent;

UCLASS()
class FPSGAME_API AFPSAICharacter : public ACharacter
{
	GENERATED_BODY()
		
public:
	// Sets default values for this character's properties
	AFPSAICharacter();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Collision")
	UBoxComponent* CatchPlayerCollision;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UPawnSensingComponent* PawnSensingComp;

protected:

	UFUNCTION()
	void HandlePlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPawnHearing(APawn* NInstigator, const FVector & Location, float Volume);

	UFUNCTION(BlueprintCallable, Category = "Perception")
	void TurnHearingPoint(FVector Point);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	
};
