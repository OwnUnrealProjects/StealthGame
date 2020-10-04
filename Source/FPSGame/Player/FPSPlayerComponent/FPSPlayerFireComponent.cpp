// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerFireComponent.h"
#include "../FPSMannequin.h"
#include "../DebugTool/DebugLog.h"

#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

#define OUT

// Sets default values for this component's properties
UFPSPlayerFireComponent::UFPSPlayerFireComponent()
{


	// ...
}


// Called when the game starts
void UFPSPlayerFireComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AFPSMannequin>(GetOwner());
	
}

FVector UFPSPlayerFireComponent::GetFireDirection(FVector start, FVector end, float speed)
{
	// FVector StartLocation = GetMesh()->GetSocketLocation("StoneStarPoint"); //Socket location not work
	// float NewSpeed = C_Result.Size();  // Such calculate Speed not work | fire location not match Aim location

	FVector LaunchVelocity = FVector::ZeroVector;

	bool SPV = UGameplayStatics::SuggestProjectileVelocity
	(
		this,
		OUT LaunchVelocity,
		start,
		end,
		speed,
		false,
		0.f,
		0.f,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);

	LOG_S(FString::Printf(TEXT("Projectile_StartStoneLocation = %s"), *start.ToString()));

	if (SPV)
	{
		LOG_S(FString("SPV = true"));
	}
	else
	{
		LOG_S(FString("SPV = false"));
	}

	DrawDebugSphere(GetWorld(), start, 25, 12, FColor::Yellow, false, 10.f, 0, 1.f);
	DrawDebugSphere(GetWorld(), end, 25, 12, FColor::Red, false, 50.f, 0, 3.f);

	return LaunchVelocity;
}



FVector UFPSPlayerFireComponent::CalculateFireDirection(FVector start, FVector end, float speed)
{
	
	FVector C_Result = end - start;
	auto NormalDirection = C_Result.GetSafeNormal();

	FRotator C_ResultRotator = NormalDirection.Rotation();
	//C_ResultRotator.Pitch = MannequinInputComponent->GetZvalue() / 2;
	//C_ResultRotator.Pitch = CameraArm->GetComponentRotation().Pitch;
	auto EndDirection = C_ResultRotator.Vector();



	DrawDebugLine(
		GetWorld(),
		start,
		end + EndDirection * 100,
		FColor(255, 0, 0),
		false,
		10.f,
		0.f,
		5.f
	);

	DrawDebugSphere(GetWorld(), start, 25, 12, FColor::Yellow, false, 10.f, 0, 1.f);
	DrawDebugSphere(GetWorld(), end, 25, 12, FColor::Red, false, 50.f, 0, 3.f);


	{
		/// Code from SuggestProjectileVelocity() function
		const FVector DirXY = C_Result.GetSafeNormal2D();
		const float DeltaXY = C_Result.Size2D();
		const float DeltaZ = C_Result.Z;
		const float TossSpeedSq = FMath::Square(1000);

		LOG_S(FString::Printf(TEXT("Calculate  C_Result = %s, Gravity = %f"), *C_Result.ToString(), GetWorld()->GetGravityZ()));
		LOG_S(FString::Printf(TEXT("Calculate  DirXY = %s, DeltaXY = %f"), *DirXY.ToString(), DeltaXY));
	}

	return C_Result;
}

