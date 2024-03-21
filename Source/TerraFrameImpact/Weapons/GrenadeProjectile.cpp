// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

AGrenadeProjectile::AGrenadeProjectile()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("爆炸物网格体"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("投射物移动组件"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
	ProjectileMovementComponent->bShouldBounce = true;
}

void AGrenadeProjectile::Destroyed()
{
	ExplodeDamage();
	Super::Destroyed();
}

void AGrenadeProjectile::BeginPlay()
{
	AActor::BeginPlay();

	SpawnTrailSystem();
	StartDestroyTimer();

	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AGrenadeProjectile::OnBounce);
	RemainBounceTimes = MaxBounceTimes;
}

void AGrenadeProjectile::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (RemainBounceTimes == 0)
	{
		Destroy();
		return;
	}
	RemainBounceTimes -= 1;
	if (BounceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BounceSound,
			GetActorLocation()
		);
	}
}
