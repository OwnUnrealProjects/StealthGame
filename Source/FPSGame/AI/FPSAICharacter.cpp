// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAICharacter.h"

#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Perception/PawnSensingComponent.h"
#include "Net/UnrealNetwork.h"

#include "../Player/FPSMannequin.h"
#include "../DebugTool/DebugLog.h"
#include "../Public/FPSGameMode.h"
#include "../Library/FPSMath.h"
#include "FPSAIGuard.h"

// Sets default values
AFPSAICharacter::AFPSAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("AIBody"));

	CatchPlayerCollision = CreateDefaultSubobject<UBoxComponent>("CatchPlayerCollision");
	CatchPlayerCollision->SetupAttachment(GetMesh(), TEXT("SocketPlayerCatchCollision"));
	CatchPlayerCollision->SetBoxExtent(FVector(20.f, 32.f, 50.f));
	CatchPlayerCollision->SetCollisionProfileName(TEXT("AICatchPlayer"));
	CatchPlayerCollision->bGenerateOverlapEvents = true;
	//CatchPlayerCollision->OnComponentBeginOverlap.AddDynamic(this, &AFPSAICharacter::HandlePlayer);


	HeadCollision = CreateDefaultSubobject<USphereComponent>(TEXT("HeadCollision"));
	HeadCollision->SetupAttachment(GetMesh());
	HeadCollision->SetCollisionProfileName(TEXT("AIHead"));

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAICharacter::OnPawnHearing);

	Tags.Add(FName("AI"));

}

void AFPSAICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSAICharacter, ShootState);
	DOREPLIFETIME(AFPSAICharacter, bHit);
	DOREPLIFETIME(AFPSAICharacter, HitDirection);
	DOREPLIFETIME(AFPSAICharacter, StoneHitDirection);
}

void AFPSAICharacter::OnPawnHearing(APawn* NInstigator, const FVector & Location, float Volume)
{
	LOG_S(FString::Printf(TEXT("Sense Hearing - Location = %s"), *Location.ToString()));
}

void AFPSAICharacter::TurnHearingPoint(FVector Point)
{
	FVector Direction = Point - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.f;
	NewLookAt.Roll = 0.f;

	SetActorRotation(NewLookAt);

	LOG_S(FString("Turn on Boss"));
}

void AFPSAICharacter::HeadShoot(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
		return;

	FName* StoneTag = OtherActor->Tags.GetData();
	FString ST = StoneTag->GetPlainNameString();
	if (ST == "Projectile")
	{
		bHit = true;
		ShootState = EShootState::HeadShoot;

		StoneHitDirection = FPSMath::GetHitPointDirectionFromRightVector(Hit.ImpactPoint, HeadCollision->GetComponentLocation(), this);
		LOG_F(StoneHitDirection);


	}
	
}

void AFPSAICharacter::BodyShoot(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
		return;

	FName* StoneTag = OtherActor->Tags.GetData();
	//FString ST = StoneTag->GetPlainNameString();
	if (StoneTag && StoneTag->ToString() == "Projectile")
	{
		LOG_S(FString("BodyHit"));

		bHit = true;
		ShootState = EShootState::BodyShoot;

		bool randomDirection = FMath::RandBool();
		if (randomDirection)
			StoneHitDirection = 180;
		if (!randomDirection)
			StoneHitDirection = 0;

	}
}

void AFPSAICharacter::HandleAlarmEvent(bool alarm, AActor* AlarmActor)
{
	//LOG_S(FString("Boss Alarm"));
	if (bFirstAlarm)
	{
		this->bAlarm = alarm;
		AlarmInstigator = AlarmActor;
		bFirstAlarm = false;
		LOG_S(FString::Printf(TEXT("AlarmActor location = %s"), *AlarmInstigator->GetActorLocation().ToString()));
	}


	if (!alarm && AlarmInstigator == AlarmActor)
	{
		this->bAlarm = alarm;
		AlarmInstigator = nullptr;
		bFirstAlarm = true;
		LOG_S(FString("AlarmInstigator is NULL"));
		LOG_I(bAlarm);
	}

}

void AFPSAICharacter::HandlePlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	LOG_S(OverlappedComponent->GetName());
	AFPSMannequin* Player = Cast<AFPSMannequin>(OtherActor);
	if (Player)
	{
		LOG_S(Player->GetName());
	}

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM && Player->IsControlled())
	{
		GM->MissionFaild(Player);
		LOG_S(FString("FPSGameMode is Not NULL"));
	}
	else
	{
		LOG_S(FString("FPSGameMode is NULL"));
	}
	
	
}

// Called when the game starts or when spawned
void AFPSAICharacter::BeginPlay()
{
	Super::BeginPlay();
	CatchPlayerCollision->OnComponentBeginOverlap.AddDynamic(this, &AFPSAICharacter::HandlePlayer);
	HeadCollision->OnComponentHit.AddDynamic(this, &AFPSAICharacter::HeadShoot);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AFPSAICharacter::BodyShoot);

	for (auto AIFreind : FreindList)
	{
		if (AIFreind)
			AIFreind->AlarmEvent.AddDynamic(this, &AFPSAICharacter::HandleAlarmEvent);
	}
}


// Called every frame
void AFPSAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
//void AFPSAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//}

