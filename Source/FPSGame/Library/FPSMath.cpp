// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSMath.h"
#include "../DebugTool/DebugLog.h"



namespace FPSMath {

	float GetHitPointDirection(FVector HitPoint, FVector CenterPoint, AActor* Actor)
	{
		FVector HitVector = CenterPoint - HitPoint;
		HitVector = HitVector.GetSafeNormal(0.01);
		float FHD = FVector::DotProduct(Actor->GetActorForwardVector(), HitVector);
		float RHD = FVector::DotProduct(Actor->GetActorRightVector(), HitVector);
		LOG_S(FString::Printf(TEXT("Head Hit Stone FHD = %f, RHD = %f"), FHD, RHD));

		float RotateDegree = 0;
		if (FMath::Abs(FHD) > FMath::Abs(RHD))
		{
			if (FHD < 0)
			{
				// Forward
				RotateDegree = 0;
			}
			else
			{
				// Backward
				RotateDegree = 180;
			}
		}
		if (FMath::Abs(RHD) > FMath::Abs(FHD))
		{
			if (RHD > 0)
			{
				// Left
				RotateDegree = 90;
			}
			else
			{
				// Right
				RotateDegree = 270;
			}
		}
		LOG_S(FString::Printf(TEXT("Head Hit Stone RotateDegree = %f"), RotateDegree));

		return RotateDegree;


		/// Get Blending with DotProduct
		if (RHD > 0)
		{
			RotateDegree = FMath::GetMappedRangeValueClamped({ -1,1 }, { 0, 180 }, FHD);
		}
		if (RHD < 0)
		{
			RotateDegree = FMath::GetMappedRangeValueClamped({ -1,1 }, { 205,360 }, FHD);
		}

		/// Convert -180 - 180 to 0 - 360 degree
		/*if (DirectionYaw > 0)
		{
			LOG_S(FString::Printf(TEXT("Head Hit Stone DirectionYaw = %f"), DirectionYaw));
		}
		else
		{
			float x = 180 + (180 + DirectionYaw);
			LOG_S(FString::Printf(TEXT("Head Hit Stone DirectionYaw = %f"), x));
		}*/
		//float DotProdact = FVector::DotProduct(GetActorForwardVector(), S);
		//FMath::GetMappedRangeValueClamped({ -1,1 }, { 0, 360 }, DotProdact);
		//DotProdact = FMath::Acos(DotProdact);
		//LOG_S(FString("Head Hit Stone ======================================================"));
		//LOG_S(FString::Printf(TEXT("Head Hit Stone DotProdact degree S & FR Acos = %f"), DotProdact));
	}

	float GetHitPointDirectionFromForwardVector(FVector HitPoint, FVector CenterPoint, AActor* Actor)
	{
		FVector HitVector = CenterPoint - HitPoint;
		HitVector = HitVector.GetSafeNormal(0.01);
		float FHD = FVector::DotProduct(Actor->GetActorForwardVector(), HitVector);

		float RotateDegree = 0;
		if (FHD <= 0)
			RotateDegree = 0;
		if (FHD > 0)
			RotateDegree = 180;

		return RotateDegree;
	}




	float GetHitPointDirectionFromRightVector(FVector HitPoint, FVector CenterPoint, AActor* Actor)
	{
		FVector HitVector = CenterPoint - HitPoint;
		HitVector = HitVector.GetSafeNormal(0.01);
		float RHD = FVector::DotProduct(Actor->GetActorRightVector(), HitVector);

		float RotateDegree = 0;
		if (RHD <= 0)
			RotateDegree = 0;
		if (RHD > 0)
			RotateDegree = 180;

		return RotateDegree;
	}

};