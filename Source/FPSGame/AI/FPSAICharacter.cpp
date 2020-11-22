// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAICharacter.h"

#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Perception/PawnSensingComponent.h"

#include "../Player/FPSMannequin.h"
#include "../DebugTool/DebugLog.h"
#include "../Public/FPSGameMode.h"

// Sets default values
AFPSAICharacter::AFPSAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CatchPlayerCollision = CreateDefaultSubobject<UBoxComponent>("CatchPlayerCollision");
	CatchPlayerCollision->SetupAttachment(GetMesh(), TEXT("SocketCollision"));
	CatchPlayerCollision->SetBoxExtent(FVector(20.f, 32.f, 50.f));
	CatchPlayerCollision->SetCollisionProfileName(TEXT("AICatchPlayer"));
	CatchPlayerCollision->bGenerateOverlapEvents = true;
	//CatchPlayerCollision->OnComponentBeginOverlap.AddDynamic(this, &AFPSAICharacter::HandlePlayer);


	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAICharacter::OnPawnHearing);

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

