// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class AFPSMannequin;

USTRUCT()
struct FAIData
{
	GENERATED_USTRUCT_BODY()

public:

	AActor* DetectActor;
	float Distance;
};

/**
 * 
 */
UCLASS()
class FPSGAME_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	AFPSPlayerController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Possess(APawn* aPawn) override;

	//virtual void AddPitchInput(float Val) override;


	/* this funtion is called in APlayerController::PostInitializedComponents */
	virtual void InitPlayerState() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Widget")
	void OnMissionCompleted(bool bMissionSuccess);

	enum ENetRole GetOwnerCharacterRole() { return OwnerPlayerRole; }
	enum ENetRole GetOwnerCharacterRemotRole() { return OwnerPlayerRemotRole; }

	FString RoleString();
	
	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetCharacterName(FString N) { CharacterName = N; }

	UFUNCTION(BlueprintPure, Category = "Player")
	FString GetPlayerName() { return CharacterName; }

	float GetMaxAimPrecision() { return MaxAimPrecision; }
	float GetMaxLoud() { return MaxLoud; }
	float GetMaxStrenght() { return MaxStrenght; }

	UFUNCTION(BlueprintPure, Category = "Player")
	AFPSMannequin* GetOwnCharacter() { return OwnCharacter; }

	UFUNCTION(BlueprintImplementableEvent)
	void GetTargetAIDistance(float distance);

protected:

	UPROPERTY(ReplicatedUsing = OnRep_Possess, BlueprintReadWrite, Category = "Possess Pawn")
	bool bPossess = false;
	UFUNCTION()
	void OnRep_Possess();

	UFUNCTION()
	void AIDetect(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void AILoss(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


protected:
	UPROPERTY(Replicated)
	FString CharacterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	float AIDetectRadius;

private:

	enum ENetRole OwnerPlayerRole = ENetRole::ROLE_None;
	enum ENetRole OwnerPlayerRemotRole = ENetRole::ROLE_None;

	UPROPERTY(Replicated)
	float MaxAimPrecision;
	UPROPERTY(Replicated)
	float MaxLoud;
	UPROPERTY()
	float MaxStrenght;

	AFPSMannequin* OwnCharacter;

	TArray<AActor*> AIActorList;

	AActor* TargetAI;
	float TargetAIDistance;

	bool bAIDetect;

};