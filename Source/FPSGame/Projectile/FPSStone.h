// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSStone.generated.h"


class UProjectileMovementComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class FPSGAME_API AFPSStone : public AActor
{
	GENERATED_BODY()

protected:

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UStaticMeshComponent* Stone;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* StoneMovement;

	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool DrawDebugShoneHitPoint;


public:	
	// Sets default values for this actor's properties
	AFPSStone();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetStoneMovement() const { return StoneMovement; }

	void LaunchStone(float speed);


	UFUNCTION(Server, Reliable, WithValidation)
	void SR_StoneHitEffect(FHitResult HitPoint, FName EffectName);

	UFUNCTION(NetMulticast, Reliable)
	void MC_StoneHitEfect(FHitResult HitPoint, FName EffectName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	/** Stone hit Effect */
	UPROPERTY(EditDefaultsOnly, Category = "TMap")
	TMap<FName, UParticleSystem*> TM_ImpactParticle;

	UPROPERTY(EditDefaultsOnly, Category = "TMap")
	TMap<FName, USoundCue*> TM_ImpactSound;
	
};
