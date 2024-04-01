// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "TerraFrameImpact/Enums/WeaponType.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

protected:
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType = EWeaponType::EWT_AssultRifle;

	UPROPERTY(EditAnywhere)
	FString ItemName = TEXT("子弹");

	UPROPERTY(EditAnywhere)
	int32 AmmoAmount = 30;
};
