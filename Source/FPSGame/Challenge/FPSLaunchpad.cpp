// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLaunchpad.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

#include "../Public/FPSCharacter.h"
#include "../Player/FPSMannequin.h"


// Sets default values
AFPSLaunchpad::AFPSLaunchpad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComp;
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	BoxComp->SetBoxExtent(FVector(150.f));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->SetHiddenInGame(false);
	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchpad::OverlapLaunchPad);

	LaunchStrength = 1500.f;
	LaunchAngle = 40.f;
}

void AFPSLaunchpad::OverlapLaunchPad(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	FRotator PadDirection = GetActorRotation();
	UE_LOG(LogTemp, Warning, TEXT("PadRotation = %s"), *PadDirection.ToString())
	PadDirection.Normalize();
	PadDirection.Pitch += 40;
	FVector Force = PadDirection.Vector() * LaunchStrength;

	//AFPSCharacter* PlayerPawn = Cast<AFPSCharacter>(OtherActor);
	AFPSMannequin* PlayerPawn = Cast<AFPSMannequin>(OtherActor);
	if (PlayerPawn)
	{

		PlayerPawn->LaunchCharacter(Force, true, true);
		UGameplayStatics::SpawnEmitterAtLocation(this, ActiveLaunchPadEffect, GetActorLocation());
	}
	else if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		/*UPrimitiveComponent* PrimC = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent());
		PrimC->AddImpulse(Force * 1000);*/
		OtherComp->AddImpulse(Force, NAME_None, true);
		UGameplayStatics::SpawnEmitterAtLocation(this, ActiveLaunchPadEffect, GetActorLocation());
	}
}

// Called when the game starts or when spawned
void AFPSLaunchpad::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSLaunchpad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

