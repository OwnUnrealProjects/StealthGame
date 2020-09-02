// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerAiming.h"
#include "../FPSMannequin.h"
#include "../DebugTool/DebugLog.h"
#include "../../FPSGame.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"


#define OUT


static int32 DebugFireLocationsDrawing = 0;
FAutoConsoleVariableRef CVARDebugFireLocationsDrawing(
	TEXT("COOP.DebugFireLocations"),
	DebugFireLocationsDrawing,
	TEXT("Draw Fire start & End Positions"),
	ECVF_Cheat
);


// Sets default values for this component's properties
UFPSPlayerAiming::UFPSPlayerAiming()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);

	BulletSpread = 2.f;
	AimTraceName = "AimBeamEnd";
}


// Called when the game starts
void UFPSPlayerAiming::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<AFPSMannequin>(GetOwner());
	
}


// Called every frame
void UFPSPlayerAiming::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UFPSPlayerAiming::AimPoint()
{
	FVector StartLocation;
	FRotator StartRotation;
	auto PlayerCamera = Player->GetCameraComponent();

	StartLocation = PlayerCamera->GetComponentLocation();
	StartRotation = PlayerCamera->GetComponentRotation();


	FVector ShotDirection = StartRotation.Vector();

	/// Spread Bullet
	//float HalfRad = FMath::DegreesToRadians(BulletSpread);                   // Get Radian
	//ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);       // Get Random vector

	FVector EndLocation = StartLocation + (ShotDirection  * LineTraceLength);
	//LOG_F(EndLocation.Size());

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Player);
	//QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FVector TraceEndPoint = EndLocation;



	FHitResult Hit;
	bool bIsDamage = GetWorld()->LineTraceSingleByChannel(
		OUT Hit,
		OUT StartLocation,
		OUT EndLocation,
		COLLISION_AIM,
		QueryParams
	);

	PlayAimBeam(EndLocation);

	if (DebugFireLocationsDrawing > 0)
	{
		DrawDebugSphere(GetWorld(), StartLocation, 100, 12, FColor::Yellow, false, 2.f, 0, 1.f);
		DrawDebugSphere(GetWorld(), EndLocation, 100, 12, FColor::Red, false, 2.f, 0, 3.f);
	}
}

void UFPSPlayerAiming::PlayAimBeam(FVector TraceEnd)
{


	if (AimBeamEffect)
	{
		TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AimBeamEffect, Player->GetActorLocation());

		if (TraceComp)
		{
			TraceComp->SetVectorParameter(AimTraceName, TraceEnd);
		}
	}
}

