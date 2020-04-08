// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSBlackHole.h"

#include "Components/SphereComponent.h"



// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlackMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlackMeshComponent"));
	RootComponent = BlackMeshComp;
	BlackMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	


	InnerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComponent"));
	InnerSphereComp->SetSphereRadius(100);
	InnerSphereComp->SetupAttachment(BlackMeshComp);

	InnerSphereComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSBlackHole::OverlapInnerSphere);

	OutSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OutSphereCompeComponent"));
	OutSphereComp->SetSphereRadius(3000);
	OutSphereComp->SetupAttachment(RootComponent);
	

}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();

	OutSphereComp->GetOverlappingComponents(OverLappingComps);
	for (auto OverlapComp : OverLappingComps)
		UE_LOG(LogTemp, Warning, TEXT("OverlapingCOmponents Number = %i  Name = %s"), OverLappingComps.Num(), *OverlapComp->GetName())
	
}

void AFPSBlackHole::OverlapInnerSphere(UPrimitiveComponent * HitComponent, AActor * OtherActor, 
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFrameSweep, const FHitResult & SweepResult)
{
	if (OtherActor)
		OtherActor->Destroy();
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	for (int32 i = 0; i < OverLappingComps.Num(); i++)
	{
		UPrimitiveComponent* PrimComp = OverLappingComps[i];
		if (PrimComp && PrimComp->IsSimulatingPhysics())
		{
			const float Radius = OutSphereComp->GetScaledSphereRadius();
			const float Strength = -2000;

			PrimComp->AddRadialForce(GetActorLocation(), Radius, Strength, ERadialImpulseFalloff::RIF_Constant, true);
		}
	}
	

}

