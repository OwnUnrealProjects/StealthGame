// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerAiming.generated.h"


class AFPSMannequin;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPSGAME_API UFPSPlayerAiming : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFPSPlayerAiming();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	AFPSMannequin* Player = nullptr;
	UParticleSystemComponent *TraceComp;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Aiming", meta = (ClampMin = 0.f))
	float BulletSpread;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	float LineTraceLength = 1000;

protected:

	void PlayAimBeam(FVector TraceEnd);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
	FName AimTraceName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming")
	UParticleSystem* AimBeamEffect;

public:

	UParticleSystemComponent* GetTraceEffect() { return TraceComp; }
	void AimPoint();
};
