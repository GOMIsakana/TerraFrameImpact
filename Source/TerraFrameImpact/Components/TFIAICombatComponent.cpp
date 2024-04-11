// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIAICombatComponent.h"
#include "TerraFrameImpact/Weapons/Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "TerraFrameImpact/AIController/TFIAIController.h"
#include "TerraFrameImpact/Character/TFIAICharacter.h"

void UTFIAICombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HoldingWeapon == nullptr) return;
	FVector StartLocation = GetHoldingWeaponMuzzleFlash()->GetSocketLocation(HoldingWeapon->GetWeaponMesh());
	ATFIAIController* AICharacterController = Cast<ATFIAIController>(Character->GetController());
	if (AICharacterController == nullptr) return;
	FVector TargetLocation = AICharacterController->GetTargetLocation() == FVector() ? AICharacterController->GetTargetLocation() : AICharacterController->GetTargetHeadLocation();

	FHitResult HitResult;
	UWorld* World = GetWorld();
	World->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		TargetLocation,
		ECollisionChannel::ECC_Visibility
	);
	if (!HitResult.bBlockingHit)
	{
		HitResult.ImpactPoint = HitResult.TraceEnd;
	}
	HitTarget = HitResult.ImpactPoint;
}

void UTFIAICombatComponent::AIFire()
{
	ServerAIFire();
}

void UTFIAICombatComponent::ServerAIFire_Implementation()
{
	Super::Fire();
	ATFICharacter* AICharacter = Cast<ATFICharacter>(Character);
	AICharacter->PlayFireMontage(false);
}
