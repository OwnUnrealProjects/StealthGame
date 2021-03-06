// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSPlayerAiming.generated.h"


class AFPSMannequin;
class UParticleSystemComponent;

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

	UPROPERTY(EditDefaultsOnly, Category = "Aiming", meta = (ClampMin = 5000.f, ClampMax = 15000.f))
	float LineTraceLength;

	UPROPERTY(Replicated)
	FVector LineTracePoint;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
	FName AimTraceName;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
	FName S_Tangent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
	FName T_Tangent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aiming")
	UParticleSystem* AimBeamEffect;


protected:

	void SpawnTraceEffectAtLocation(FVector TraceEnd, float ParticleTangent);
	void UpdateTraceEffectLocation(FVector TraceEnd, float ParticleTangent);

	UFUNCTION(Server, Reliable, WithValidation)
	void SR_UpdateLineTracePoint(FVector EndPoint);


public:

	UParticleSystemComponent* GetTraceEffect() { return TraceComp; }
	void AimPoint(float ParticleTangent);
	void DestroyTraceEffect();

	FVector GetLineTraceEndPoint() { return LineTracePoint; }

	float GetLineTraceLength() { return LineTraceLength; }
};
