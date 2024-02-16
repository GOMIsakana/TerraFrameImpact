// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "BulletProjectile.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ABulletProjectile : public AProjectile
{
	GENERATED_BODY()
public:
	ABulletProjectile();

protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult) override;
};
