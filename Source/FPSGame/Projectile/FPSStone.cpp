// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSStone.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

#include "../DebugTool/DebugLog.h"


// Sets default values
AFPSStone::AFPSStone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(20.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AFPSStone::OnHit);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	Stone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Stone"));
	Stone->SetupAttachment(RootComponent);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	StoneMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("StoneMovement"));
	StoneMovement->bAutoActivate = false;
	//StoneMovement->InitialSpeed = 1000.f;
	/*StoneMovement->UpdatedComponent = Stone;
	StoneMovement->InitialSpeed = 1000.f;
	StoneMovement->MaxSpeed = 1500.f;
	StoneMovement->bRotationFollowsVelocity = true;*/
	StoneMovement->bShouldBounce = true;
	StoneMovement->bRotationFollowsVelocity = true;
	
	// Die after 3 seconds by default
	//InitialLifeSpan = 3.0f;

	SetReplicates(true);
	SetReplicateMovement(true);

}

void AFPSStone::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

	}

	//LOG_S(FString::Printf(TEXT("SSS Hit Actor = %s"), *OtherActor->GetName()));
	DrawDebugSphere(GetWorld(), GetActorLocation(), 25, 12, FColor::Green, false, 10.f, 0, 1.f);

	if (Role == ROLE_Authority)
	{
		MakeNoise(1.f, Instigator); // Instigator set Fire() function
		LOG_S(FString::Printf(TEXT("Sense Stone Hit Location = %s"), *GetActorLocation().ToString()));
		Destroy();
	}
}

void AFPSStone::LaunchStone(float speed)
{
	//const FRotator Rotation = GetActorRotation();
	//const FRotator YawRotation(0, Rotation.Yaw, 0);

	//LOG_S(FString::Printf(TEXT("ProjectileCameraDirection = %s"), *Veloctity.ToString()));
	// get forward vector
	//const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	//StoneMovement->InitialSpeed = 1000.f;
	//StoneMovement->SetVelocityInLocalSpace((FVector::ForwardVector + FVector(0,0,StoneVelocity.Z * 5)) * 1500);
	StoneMovement->SetVelocityInLocalSpace(FVector::ForwardVector * speed);
	//StoneMovement->SetVelocityInLocalSpace(-Veloctity * 1500);
	StoneMovement->Activate();
}

// Called when the game starts or when spawned
void AFPSStone::BeginPlay()
{
	Super::BeginPlay();
	//Stone->OnComponentHit.AddDynamic(this, &AFPSStone::OnHit);
}

// Called every frame
void AFPSStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

