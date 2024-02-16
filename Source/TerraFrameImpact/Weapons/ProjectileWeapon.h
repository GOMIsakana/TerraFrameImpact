// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "GameFramework/Actor.h"
#include "Projectile.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()
	
public:
	virtual void Fire(const FVector& TargetPos) override;

private:
	UPROPERTY(EditAnywhere, Category = "射弹武器资产")
	TSubclassOf<AProjectile> ProjectileClass;
};
