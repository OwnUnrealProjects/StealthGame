// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSStone.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Containers/Map.h"
//#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"

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

	Tags.Add(TEXT("Projectile"));

	SetReplicates(true);
	SetReplicateMovement(true);

	bNetLoadOnClient = true;


}



void AFPSStone::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

	}

	if (DrawDebugShoneHitPoint)
	{
		DrawDebugSphere(GetWorld(), GetActorLocation(), 25, 12, FColor::Green, false, 10.f, 0, 1.f);
		DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 50.f), OtherComp->GetName(), nullptr, FColor::Purple, 10.f);
	}
	

	/*if (Role == ROLE_Authority)
		LOG_S(FString::Printf(TEXT("Server HitPoint = %s"), *StoneHit.ImpactPoint.ToString()));
	if (Role == ROLE_AutonomousProxy)
		LOG_S(FString::Printf(TEXT("Client HitPoint = %s"), *StoneHit.ImpactPoint.ToString()));
	if (Role == ROLE_SimulatedProxy)
		LOG_S(FString::Printf(TEXT("Simulate HitPoint = %s"), *StoneHit.ImpactPoint.ToString()));*/
	
	if (OtherActor && OtherActor->Tags.Num() != 0)
	{
		FName TM_Key = OtherActor->Tags[0];
		bool IsEmitter = TM_ImpactParticle.Contains(TM_Key);
		if (IsEmitter)
			SR_StoneHitEffect(Hit, TM_Key);
	
	}

	if (Role == ROLE_Authority)
	{
		
		MakeNoise(1.f, Instigator, this->GetActorLocation(), 100.f); // Instigator set Fire() function
		LOG_S(FString::Printf(TEXT("Sense Stone Hit Location = %s"), *GetActorLocation().ToString()));
		Destroy();
	}
	
}


void AFPSStone::SR_StoneHitEffect_Implementation(FHitResult HitPoint, FName EffectName)
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TM_ImpactParticle[EffectName], HitPoint.ImpactPoint, HitPoint.ImpactNormal.Rotation());
	UGameplayStatics::PlaySound2D(
		GetWorld(),
		TM_ImpactSound[EffectName],
		FMath::RandRange(0.5f, 0.9f),
		1.f,
		FMath::RandRange(0.1f, 0.3f),
		nullptr,
		this
		);
	MC_StoneHitEfect(HitPoint, EffectName);
}

bool AFPSStone::SR_StoneHitEffect_Validate(FHitResult HitPoint, FName EffectName)
{
	return true;
}

void AFPSStone::MC_StoneHitEfect_Implementation(FHitResult HitPoint, FName EffectName)
{
	//if(Role == ROLE_SimulatedProxy)
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TM_ImpactParticle[EffectName], HitPoint.ImpactPoint, HitPoint.ImpactNormal.Rotation());
	UGameplayStatics::PlaySound2D(
		GetWorld(),
		TM_ImpactSound[EffectName],
		FMath::RandRange(0.5f, 0.9f),
		1.f,
		FMath::RandRange(0.1f, 0.3f),
		nullptr,
		this
	);

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

	/*if (Role == ROLE_Authority)
		LOG_S(FString::Printf(TEXT("Server HitPoint = %s"), *StoneHit.ImpactPoint.ToString()));
	if (Role == ROLE_AutonomousProxy)
		LOG_S(FString::Printf(TEXT("Client HitPoint = %s"), *StoneHit.ImpactPoint.ToString()));
	if (Role == ROLE_SimulatedProxy)
		LOG_S(FString::Printf(TEXT("Simulate HitPoint = %s"), *StoneHit.ImpactPoint.ToString()));*/
}

// Called every frame
void AFPSStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (Role == ROLE_Authority)
		LOG_S(FString("Server"));
	if (Role == ROLE_AutonomousProxy)
		LOG_S(FString("Client"));
	if (Role == ROLE_SimulatedProxy)
		LOG_S(FString("Simulated"));;*/

}

