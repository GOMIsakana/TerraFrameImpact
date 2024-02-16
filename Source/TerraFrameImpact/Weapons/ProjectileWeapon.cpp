// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& TargetPos)
{
	Super::Fire(TargetPos);

	const USkeletalMeshSocket* MuzzleSocket = WeaponMesh->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	if (World && MuzzleSocket)
	{
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(WeaponMesh);
		FVector ToTarget = TargetPos - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			World->SpawnActor<AProjectile>(
				ProjectileClass,
				SocketTransform.GetLocation(),
				TargetRotation,
				SpawnParams
			);
		}
	}
}
