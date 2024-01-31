// Fill out your copyright notice in the Description page of Project Settings.


#include "TFICombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "TerraFrameImpact/Enums/WeaponState.h"
#include "TerraFrameImpact/Weapons/Weapon.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values for this component's properties
UTFICombatComponent::UTFICombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UTFICombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTFICombatComponent, HoldingWeapon);
	DOREPLIFETIME(UTFICombatComponent, bIsAiming);
	DOREPLIFETIME(UTFICombatComponent, bIsDashing);
}

// Called when the game starts
void UTFICombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UTFICombatComponent::OnRep_IsAiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bIsAiming = bAimButtonPressed;
	}
}

void UTFICombatComponent::AttachActorToCharacterMesh(AActor* OtherActor, FName SocketName)
{
	if (Character == nullptr) return;
	const USkeletalMeshSocket* MeshSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (MeshSocket != nullptr)
	{
		MeshSocket->AttachActor(OtherActor, Character->GetMesh());
	}
}

void UTFICombatComponent::EquipWeapon(AWeapon* TargetWeapon)
{
	// 这里是“当且仅当自己是服务器”的时候会去执行
	if (TargetWeapon == nullptr || Character == nullptr) return;
	if (HoldingWeapon != nullptr)
	{
		DropWeapon();
	}
	HoldingWeapon = TargetWeapon;
	HoldingWeapon->SetOwner(Character);
	HoldingWeapon->SetWeaponState(EWeaponState::EWS_HasOwner);
	HoldingWeapon->ShowPickupWidget(false);
	if (Character->GetCharacterMovement() != nullptr)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	Character->bUseControllerRotationYaw = true;
	AttachActorToCharacterMesh(HoldingWeapon, FName("WeaponSocket"));

}

void UTFICombatComponent::DropWeapon()
{
	if (HoldingWeapon == nullptr) return;
	FDetachmentTransformRules DetachRule(EDetachmentRule::KeepWorld, true);
	HoldingWeapon->DetachFromActor(DetachRule);
	HoldingWeapon->SetOwner(nullptr);
	HoldingWeapon->SetWeaponState(EWeaponState::EWS_Dropped);
}

void UTFICombatComponent::OnRep_HoldingWeapon()
{
	// 这里是“自己不是服务器”的时候会去本地执行
	if (HoldingWeapon == nullptr || Character == nullptr) return;
	HoldingWeapon->SetOwner(Character);
	HoldingWeapon->SetWeaponState(EWeaponState::EWS_HasOwner);
	HoldingWeapon->ShowPickupWidget(false);
	if (Character->GetCharacterMovement() != nullptr)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	Character->bUseControllerRotationYaw = true;
	AttachActorToCharacterMesh(HoldingWeapon, FName("WeaponSocket"));
}

void UTFICombatComponent::Dash(bool state)
{
	if (bIsAiming) return;
	bIsDashing = state;
	// 本地不设的话会导致鬼畜
	if (state)
	{
		if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
		bIsDashing = true;
		Character->GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
		ServerStartDash();
	}
	else
	{
		if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
		bIsDashing = false;
		Character->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		ServerStopDash();
	}
}


void UTFICombatComponent::ServerStartDash_Implementation()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	bIsDashing = true;
	Character->GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
}

void UTFICombatComponent::ServerStopDash_Implementation()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	bIsDashing = false;
	Character->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void UTFICombatComponent::ServerSetAiming_Implementation(bool state)
{
	bIsAiming = state;
	if (Character && Character->GetCharacterMovement())
	{
		if (bIsAiming)
		{
			Dash(false);
			Character->GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
		}
		else
		{
			Character->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
			if (bDashButtonPressed)
			{
				Dash(true);
			}
		}
	}
}

void UTFICombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UTFICombatComponent::SetAiming(bool state)
{
	if (HoldingWeapon == nullptr) return;
	bIsAiming = state;
	ServerSetAiming(bIsAiming);
	if (bIsAiming)
	{
		Dash(false);
		Character->GetCharacterMovement()->MaxWalkSpeed = AimingSpeed;
	}
	else
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		if (bDashButtonPressed)
		{
			Dash(true);
		}
	}
	if (Character && Character->IsLocallyControlled()) bAimButtonPressed = bIsAiming;
}

void UTFICombatComponent::SetDashButtonPressed(bool state)
{
	bDashButtonPressed = state;
	Dash(bDashButtonPressed);
}

