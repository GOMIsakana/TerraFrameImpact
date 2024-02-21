// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "ShotgunWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API AShotgunWeapon : public AHitScanWeapon
{
	GENERATED_BODY()
	
public:
	virtual void FireShotgun(const TArray<FVector_NetQuantize>& HitTargets);
	void ShotgunTraceEndWithSpread(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);

private:
	UPROPERTY(EditAnywhere, Category = "武器属性")
	uint32 NumberOfBullets = 5;

public:
	FORCEINLINE uint32 GetNumberOfBullets() const { return NumberOfBullets; }
};
