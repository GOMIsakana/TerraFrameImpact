// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "Kismet/GameplayStatics.h"

ABulletProjectile::ABulletProjectile()
{
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("投射物移动组件"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->SetIsReplicated(true);
}

void ABulletProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		AController* OwnerController = OwnerCharacter->Controller;
		if (OwnerController && OtherActor != OwnerController)
		{
			UGameplayStatics::ApplyDamage(
				OtherActor,
				Damage,
				OwnerController,
				this,
				UDamageType::StaticClass()
			);
		}
	}
	Super::OnHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, HitResult);
}
