// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSPlayerAiming.h"
#include "../FPSMannequin.h"
#include "../DebugTool/DebugLog.h"
#include "../../FPSGame.h"

#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

//#define LOG_S
#define OUT




// Sets default values for this component's properties
UFPSPlayerAiming::UFPSPlayerAiming()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	//SetIsReplicated(true);

	AimTraceName = "AimBeamEnd";
	S_Tangent = "Source_Tangent";
	T_Tangent = "Target_Tangent";
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

}

void UFPSPlayerAiming::AimPoint(float ParticleTangent)
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
	bool LTSC = GetWorld()->LineTraceSingleByChannel(
		OUT Hit,
		OUT StartLocation,
		OUT EndLocation,
		COLLISION_AIM,
		QueryParams
	);

	if (LTSC)
	{
		LineTracePoint = Hit.Location;
		SR_UpdateLineTracePoint(Hit.Location);
		//LOG_S(FString::Printf(TEXT("LTSC Hit = %s"), *Hit.GetActor()->GetName()));
	}
	else
	{
		LineTracePoint = EndLocation;
		SR_UpdateLineTracePoint(EndLocation);
		LOG_S(FString("LTSC = false"));
	}

	if (TraceComp/* && TraceComp->bIsActive*/)
	{
		UpdateTraceEffectLocation(LineTracePoint, ParticleTangent);
		LOG_S(FString("TraceComp is Updated"));
	}
	else
	{
		SpawnTraceEffectAtLocation(LineTracePoint, ParticleTangent);
		//TraceComp->bIsActive = true;
		LOG_S(FString("TraceComp is Spawned"));
	}
	



}

void UFPSPlayerAiming::DestroyTraceEffect()
{
	/*TraceComp->bIsActive = false;
	TraceComp->Deactivate();*/
	TraceComp->DestroyComponent();
	TraceComp = nullptr;
}

void UFPSPlayerAiming::SpawnTraceEffectAtLocation(FVector TraceEnd, float ParticleTangent)
{


	if (AimBeamEffect)
	{
		TraceComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), AimBeamEffect, Player->GetMesh()->GetSocketLocation("StartBeamEffect"));
	
		if (TraceComp)
		{
			TraceComp->SetVectorParameter(AimTraceName, TraceEnd);
			TraceComp->SetVectorParameter(S_Tangent, FVector(0, 0, ParticleTangent));
			TraceComp->SetVectorParameter(T_Tangent, FVector(0, 0, -ParticleTangent));
		}
	}
}

void UFPSPlayerAiming::UpdateTraceEffectLocation(FVector TraceEnd, float ParticleTangent)
{
	if (TraceComp)
	{
		TraceComp->SetVectorParameter(AimTraceName, TraceEnd);
		TraceComp->SetVectorParameter(S_Tangent, FVector(0, 0, ParticleTangent));
		TraceComp->SetVectorParameter(T_Tangent, FVector(0, 0, -ParticleTangent));
	}
}




void UFPSPlayerAiming::SR_UpdateLineTracePoint_Implementation(FVector EndPoint)
{
	LineTracePoint = EndPoint;
}

bool UFPSPlayerAiming::SR_UpdateLineTracePoint_Validate(FVector EndPoint)
{
	return true;
}

void UFPSPlayerAiming::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFPSPlayerAiming, LineTracePoint);

}
