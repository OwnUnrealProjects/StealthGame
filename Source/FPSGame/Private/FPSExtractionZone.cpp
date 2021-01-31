// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSExtractionZone.h"

#include "Components/BoxComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

#include "FPSCharacter.h"
#include "FPSPlayerController.h"
//#include "../Game/FPSInGameMode.h"
#include "../Public/FPSGameMode.h"
#include "../DebugTool/DebugLog.h"


// Sets default values
AFPSExtractionZone::AFPSExtractionZone()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComponent"));
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetBoxExtent(FVector(200.f));
	RootComponent = BoxComp;

	BoxComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSExtractionZone::HandleOverlap);

	// For Debug
	BoxComp->SetHiddenInGame(false);


	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComponent"));
	DecalComp->DecalSize = FVector(200.f);
	DecalComp->SetupAttachment(RootComponent);

	
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

void AFPSExtractionZone::HandleOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	//AFPSCharacter* PlayerPawn = Cast<AFPSCharacter>(OtherActor);
	AFPSMannequin* PlayerPawn = Cast<AFPSMannequin>(OtherActor);
	if (!PlayerPawn)
		return;
	
	
	UE_LOG(LogTemp, Warning, TEXT("Extraction Zone Player name = %s,  CarryingObjective = %i"), *PlayerPawn->GetName(), PlayerPawn->bIsCarryingObjective)
	if (PlayerPawn->bIsCarryingObjective)
	{
		/// GetAuthGameMode() == that this function will be executed only for Server
		/// this is meaningful only MultiPlayer Game. In MultiPlayer Game this function Executed only Server
		/// for Client GetAuthGameMode() = NULL
		UE_LOG(LogTemp, Warning, TEXT("Extraction CarryingObjective Area Zone Player name = %s,  CarryingObjective = %i"), *PlayerPawn->GetName(), PlayerPawn->bIsCarryingObjective)
		AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			//here is coming only Server because GameMode is Exist only on Server
			GM->CompleteMission(); // true
		}
		
		
	}
	else
	{
		/// GamePlayStatics is a Class which include all Static functions
		/// Some of that function returns UObject type but in case that static functions are singleton
		/// it's return Object does not exists in Level
		UGameplayStatics::PlaySound2D(this, ObjectiveMissingSOund);
		FString role = GetEnumText(PlayerPawn->Role);
		UE_LOG(LogTemp, Warning, TEXT("Extraction Zone Player name = %s,  Player Role = %s"), *PlayerPawn->GetName(), *role);
	
	}

	


}


// Called when the game starts or when spawned
void AFPSExtractionZone::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSExtractionZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

