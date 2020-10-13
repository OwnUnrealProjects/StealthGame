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

	/*bool SPV = UGameplayStatics::SuggestProjectileVelocity
	(
		Player,
		OUT LaunchVelocity,
		start,
		end,
		speed,
		false,
		0.f,
		0.f,
		ESuggestProjVelocityTraceOption::DoNotTrace
	);*/

	bool SPV = SuggestStoneVelocity(LaunchVelocity, start, end, speed, false);

	LOG_S(FString::Printf(TEXT("EEE Projectile_StartStoneLocation = %s"), *start.ToString()));
	LOG_S(FString::Printf(TEXT("EEE Projectile_EndLocation = %s"), *end.ToString()));

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



bool UFPSPlayerFireComponent::SuggestStoneVelocity(FVector& OutTossVelocity, FVector Start, FVector End, float TossSpeed, bool bFavorHighArc)
{
	const FVector FlightDelta = End - Start;
	const FVector DirXY = FlightDelta.GetSafeNormal2D();
	const float DeltaXY = FlightDelta.Size2D();

	const float DeltaZ = FlightDelta.Z;

	const float TossSpeedSq = FMath::Square(TossSpeed);

	/*const UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
	{
		return false;
	}*/
	const float GravityZ = -GetWorld()->GetGravityZ();;

	// v^4 - g*(g*x^2 + 2*y*v^2)
	const float InsideTheSqrt = FMath::Square(TossSpeedSq) - GravityZ * ((GravityZ * FMath::Square(DeltaXY)) + (2.f * DeltaZ * TossSpeedSq));
	if (InsideTheSqrt < 0.f)
	{
		// sqrt will be imaginary, therefore no solutions
		return false;
	}

	// if we got here, there are 2 solutions: one high-angle and one low-angle.

	const float SqrtPart = FMath::Sqrt(InsideTheSqrt);

	// this is the tangent of the firing angle for the first (+) solution
	const float TanSolutionAngleA = (TossSpeedSq + SqrtPart) / (GravityZ * DeltaXY);
	// this is the tangent of the firing angle for the second (-) solution
	const float TanSolutionAngleB = (TossSpeedSq - SqrtPart) / (GravityZ * DeltaXY);

	// mag in the XY dir = sqrt( TossSpeedSq / (TanSolutionAngle^2 + 1) );
	const float MagXYSq_A = TossSpeedSq / (FMath::Square(TanSolutionAngleA) + 1.f);
	const float MagXYSq_B = TossSpeedSq / (FMath::Square(TanSolutionAngleB) + 1.f);

	bool bFoundAValidSolution = false;

	// trace if desired
	
	// choose which arc
	const float FavoredMagXYSq = bFavorHighArc ? FMath::Min(MagXYSq_A, MagXYSq_B) : FMath::Max(MagXYSq_A, MagXYSq_B);
	const float ZSign = bFavorHighArc ?
		(MagXYSq_A < MagXYSq_B) ? FMath::Sign(TanSolutionAngleA) : FMath::Sign(TanSolutionAngleB) :
		(MagXYSq_A > MagXYSq_B) ? FMath::Sign(TanSolutionAngleA) : FMath::Sign(TanSolutionAngleB);

	// finish calculations
	const float MagXY = FMath::Sqrt(FavoredMagXYSq);
	const float MagZ = FMath::Sqrt(TossSpeedSq - FavoredMagXYSq);		// pythagorean

	// final answer!
	OutTossVelocity = (DirXY * MagXY) + (FVector::UpVector * MagZ * ZSign);
	bFoundAValidSolution = true;

	return bFoundAValidSolution;
	
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

