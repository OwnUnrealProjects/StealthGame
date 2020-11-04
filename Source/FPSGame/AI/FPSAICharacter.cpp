// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAICharacter.h"

#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"

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
	//CatchPlayerCollision->OnComponentHit.AddDynamic(this, &AFPSAICharacter::HandlePlayer);

}

void AFPSAICharacter::HandlePlayer(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	LOG_S(FString("Boss HitComponent"));
	auto Player = Cast<AFPSMannequin>(OtherActor);
	if (Player)
	{
		LOG_S(Player->GetName());
	}

	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	GM->MissionFaild(Player);
	
}

// Called when the game starts or when spawned
void AFPSAICharacter::BeginPlay()
{
	Super::BeginPlay();
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

