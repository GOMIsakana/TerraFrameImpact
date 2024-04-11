// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"

void AProjectileWeapon::Fire(const FVector& TargetPos)
{
	Super::Fire(TargetPos);

	const USkeletalMeshSocket* MuzzleSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	UWorld* World = GetWorld();
	if (World && MuzzleSocket)
	{
		APawn* InstigatorPawn = Cast<APawn>(GetOwner());
		FTransform SocketTransform = MuzzleSocket->GetSocketTransform(GetWeaponMesh());
		// UE_LOG(LogTemp, Warning, TEXT("SocketLocation: X = %.2f, Y = %.2f, Z = %.2f"), SocketTransform.GetLocation().X, SocketTransform.GetLocation().Y, SocketTransform.GetLocation().Z );
		FVector ToTarget = TargetPos - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if (ProjectileClass && InstigatorPawn)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			AProjectile* NewProjectile = World->SpawnActor<AProjectile>(
				ProjectileClass,
				SocketTransform.GetLocation(),
				TargetRotation,
				SpawnParams
			);
			/*if (NewProjectile)
			{
				UE_LOG(LogTemp, Warning, TEXT("NewProjectile: X = %.2f, Y = %.2f, Z = %.2f"), NewProjectile->GetActorLocation().X, NewProjectile->GetActorLocation().Y, NewProjectile->GetActorLocation().Z);
			}*/
		}
	}
}
