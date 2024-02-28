// Fill out your copyright notice in the Description page of Project Settings.


#include "TFICombatComponent.h"
#include "Net/UnrealNetwork.h"
#include "TerraFrameImpact/Enums/WeaponState.h"
#include "TerraFrameImpact/Weapons/Weapon.h"
#include "TerraFrameImpact/Weapons/ShotgunWeapon.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TerraFrameImpact/Enums/CharacterState.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "TerraFrameImpact/HUD/TFICharacterHUD.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"

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
	DOREPLIFETIME(UTFICombatComponent, CharacterState);
	DOREPLIFETIME(UTFICombatComponent, BulletJumpFacingRotator);
	DOREPLIFETIME_CONDITION(UTFICombatComponent, CurrentCarriedAmmo, COND_OwnerOnly);
}

// Called when the game starts
void UTFICombatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentBulletJumpLimit = BulletJumpLimit;
}

void UTFICombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrossHair(HitResult);
		// 有时候会出现“对着墙也会找不到ImpactPoint”的情况，需要解决
		if (HitResult.bBlockingHit)
		{
			HitTarget = HitResult.ImpactPoint;
		}
		else
		{
			HitTarget = HitResult.TraceEnd;
		}
		SetHUDCrosshairs(DeltaTime);

		if (CharacterState == ECharacterState::ECS_Normal && (Character->GetActorRotation().Pitch != 0.f || Character->GetActorRotation().Roll != 0.f))
		{
			BulletJumpCompleted();
		}
	}
}

void UTFICombatComponent::OnRep_IsAiming()
{
	if (Character && Character->IsLocallyControlled())
	{
		bIsAiming = bAimButtonPressed;
	}
}

void UTFICombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;

	Controller = Controller == nullptr ? Cast<ATFIPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ATFICharacterHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			if (HoldingWeapon)
			{
				HUDPackage.CrosshairsCenter = HoldingWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = HoldingWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = HoldingWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = HoldingWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = HoldingWeapon->CrosshairsBottom;
			}
			else
			{
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			// 计算扩散
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairFallingFactor = FMath::FInterpTo(CrosshairFallingFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairFallingFactor = FMath::FInterpTo(CrosshairFallingFactor, 0.f, DeltaTime, 2.25f);
			}
			if (bIsAiming)
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, .5f, DeltaTime, 30.f);
			}
			else
			{
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshairShootFactor = FMath::FInterpTo(CrosshairShootFactor, 0.f, DeltaTime, 10.f);

			HUDPackage.CrosshairSpread =
				.5f +
				CrosshairVelocityFactor +
				CrosshairFallingFactor -
				CrosshairAimFactor +
				CrosshairShootFactor;

			HUD->SetCrosshairPackage(HUDPackage);
		}
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
	if (TargetWeapon == nullptr || Character == nullptr) return;
	if (HoldingWeapon != nullptr)
	{
		DropWeapon();
	}
	HoldingWeapon = TargetWeapon;
	HoldingWeapon->SetOwner(Character);
	HoldingWeapon->SetWeaponState(EWeaponState::EWS_HasOwner);
	HoldingWeapon->ShowPickupWidget(false);
	HoldingWeapon->SetHUDAmmo();
	if (Character->HasAuthority())
	{
		InitCarriedAmmo();
	}
	UpdateCarriedValue();
	if (Character->GetCharacterMovement() != nullptr)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	Character->bUseControllerRotationYaw = true;
	AttachActorToCharacterMesh(HoldingWeapon, FName("WeaponSocket"));
	ReloadEmptyWeapon();
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
	HoldingWeapon->SetHUDAmmo();
	if (Character->GetCharacterMovement() != nullptr)
	{
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	Character->bUseControllerRotationYaw = true;
	AttachActorToCharacterMesh(HoldingWeapon, FName("WeaponSocket"));
}

void UTFICombatComponent::Dash(bool state)
{
	if (bIsAiming || bPreparingBattle) return;
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
	Character->GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
}

void UTFICombatComponent::ServerStopDash_Implementation()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	Character->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

// 根据当前屏幕大小进行一次射线检测，获取到屏幕中心现在指向的世界位置
void UTFICombatComponent::TraceUnderCrossHair(FHitResult& HitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D ScreenCenterPosition(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector ScreenWorldPosition;
	FVector ScreenWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),	// 获取第0个玩家控制器
		ScreenCenterPosition,
		ScreenWorldPosition,
		ScreenWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector TraceStart = ScreenWorldPosition;
		if (Character)
		{
			float ScreenToCharacterDistance = (Character->GetActorLocation() - TraceStart).Size();
			TraceStart += ScreenWorldDirection * (ScreenToCharacterDistance + 100.f);
		}

		FVector TraceEnd = TraceStart + ScreenWorldDirection * 80000.f;

		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECollisionChannel::ECC_Visibility
		);
	}
}

void UTFICombatComponent::Fire()
{
	if (!CanFire()) return;
	Dash(false);

	bCanFire = false;
	if (Character)
	{
		FVector Velocity = Character->GetVelocity();
		Velocity.Z = 0.f;
		float Speed = Velocity.Size();
		if (Speed > 0.f || Character->GetAO_YawOutofRange())
		{
			StartPreparingBattleTimer();
		}
	}
	
	if (HoldingWeapon != nullptr)
	{
		CrosshairShootFactor = .75f;
		switch (HoldingWeapon->GetFireType())
		{
		case EFireType::EFT_Projectile:
			FireProjectileWeapon();
			break;
		case EFireType::EFT_HitScan:
			FireHitScanWeapon();
			break;
		case EFireType::EFT_MultiHitScan:
			FireShotgunWeapon();
			break;
		default:
			break;
		}
	}
	StartFireTimer();
}

void UTFICombatComponent::FireProjectileWeapon()
{
	if (HoldingWeapon == nullptr || Character == nullptr) return;
	HitTarget = HoldingWeapon->bEnableSpread ? HoldingWeapon->TraceEndWithSpread(HitTarget) : HitTarget;
	if (!Character->HasAuthority()) LocalFire(HitTarget);
	ServerFire(HitTarget);
}

void UTFICombatComponent::FireHitScanWeapon()
{
	if (HoldingWeapon == nullptr || Character == nullptr) return;
	HitTarget = HoldingWeapon->bEnableSpread ? HoldingWeapon->TraceEndWithSpread(HitTarget) : HitTarget;
	if (!Character->HasAuthority()) LocalFire(HitTarget);
	ServerFire(HitTarget);
}

void UTFICombatComponent::FireShotgunWeapon()
{
	AShotgunWeapon* Weapon = Cast<AShotgunWeapon>(HoldingWeapon);
	if (Weapon == nullptr || Character == nullptr) return;
	TArray<FVector_NetQuantize> HitTargets;
	if (Weapon->bEnableSpread)
	{
		Weapon->ShotgunTraceEndWithSpread(HitTarget, HitTargets);
	}
	else
	{
		for (uint32 i = 0; i < Weapon->GetNumberOfBullets(); i++)
		{
			HitTargets.Add(HitTarget);
		}
	}
	if (!Character->HasAuthority()) LocalShotgunFire(HitTargets);
	ServerShotgunFire(HitTargets);
}

void UTFICombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TargetPos)
{
	MulticastFire_Implementation(TargetPos);
	/*
	if (Character == nullptr) return;
	FRotator CharacterRotation(0.f, Character->GetBaseAimRotation().Yaw, 0.f);
	Character->SetActorRotation(CharacterRotation);
	*/
}

void UTFICombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TargetPos)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	// 这里是为了只让想开枪的玩家，或者服务器上的玩家开枪
	LocalFire(TargetPos);
}

void UTFICombatComponent::LocalFire(const FVector_NetQuantize& TargetPos)
{
	if (HoldingWeapon == nullptr) return;
	if (Character && CharacterState == ECharacterState::ECS_Normal)
	{
		Character->PlayFireMontage(bIsAiming);
		HoldingWeapon->Fire(TargetPos);
	}
}

void UTFICombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TargetPosList)
{
	MulticastShotgunFire(TargetPosList);
}

void UTFICombatComponent::MulticastShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TargetPosList)
{
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	LocalShotgunFire(TargetPosList);
}

void UTFICombatComponent::LocalShotgunFire(const TArray<FVector_NetQuantize>& TargetPosList)
{
	if (HoldingWeapon == nullptr) return;
	if (Character && CharacterState == ECharacterState::ECS_Normal)
	{
		Character->PlayFireMontage(bIsAiming);
		AShotgunWeapon* Weapon = Cast<AShotgunWeapon>(HoldingWeapon);
		Weapon->FireShotgun(TargetPosList);
	}
}

void UTFICombatComponent::StartFireTimer()
{
	if (HoldingWeapon == nullptr || Character == nullptr) return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UTFICombatComponent::FireTimerFinished,
		HoldingWeapon->FireDelay
	);
}

void UTFICombatComponent::FireTimerFinished()
{
	if (HoldingWeapon == nullptr) return;
	bCanFire = true;
	if (bFireButtonPressed && HoldingWeapon->bAutomatic)
	{
		Fire();
	}
	ReloadEmptyWeapon();
}

void UTFICombatComponent::ReloadWeapon()
{
	if (CurrentCarriedAmmo <= 0 || HoldingWeapon == nullptr || CharacterState != ECharacterState::ECS_Normal || HoldingWeapon->IsFull()) return;
	ServerReload();
	LocalReload();
	bIsReloading = true;
}

void UTFICombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || HoldingWeapon == nullptr) return;

	CharacterState = ECharacterState::ECS_Reload;
	if (!Character->IsLocallyControlled()) LocalReload();
}

void UTFICombatComponent::StartPreparingBattleTimer()
{
	if (Character == nullptr) return;

	GetWorld()->GetTimerManager().SetTimer(
		PreparingBattleTimer,
		this,
		&UTFICombatComponent::PreparingBattleTimerFinished,
		5.f
	);
	bPreparingBattle = true;
}

void UTFICombatComponent::ClearPreparingBattleTimer()
{
	bPreparingBattle = false;
	Dash(bDashButtonPressed);
	GetWorld()->GetTimerManager().ClearTimer(PreparingBattleTimer);
}

void UTFICombatComponent::PreparingBattleTimerFinished()
{
	bPreparingBattle = false;
	Dash(bDashButtonPressed);
}

void UTFICombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ATFIPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CurrentCarriedAmmo);
	}
}

void UTFICombatComponent::LocalReload()
{
	if (Character && HoldingWeapon)
	{
		Character->PlayReloadMontage(HoldingWeapon->GetWeaponType());
	}
}

int32 UTFICombatComponent::AmountToReload()
{
	if (HoldingWeapon == nullptr) return 0;
	int32 MagSpace = HoldingWeapon->GetMagCapacity() - HoldingWeapon->GetAmmo();

	if (CarriedAmmoMap.Contains(HoldingWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[HoldingWeapon->GetWeaponType()];
		int32 Least = FMath::Min(MagSpace, AmountCarried);
		return FMath::Clamp(MagSpace, 0, Least);
	}
	return 0;
}

void UTFICombatComponent::ReloadEmptyWeapon()
{
	if (HoldingWeapon && HoldingWeapon->IsEmpty())
	{
		ReloadWeapon();
	}
}

void UTFICombatComponent::ReloadCompleted()
{
	if (Character == nullptr) return;
	bIsReloading = false;
	if (Character->HasAuthority())
	{
		CharacterState = ECharacterState::ECS_Normal; 
		UpdateAmmoValue();
	}
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UTFICombatComponent::InitCarriedAmmo()
{
	if (HoldingWeapon == nullptr) return;
	CarriedAmmoMap.Emplace(HoldingWeapon->GetWeaponType(), HoldingWeapon->GetCarriedAmmo());
}

void UTFICombatComponent::UpdateAmmoValue()
{
	if (Character == nullptr || HoldingWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload();
	if (CarriedAmmoMap.Contains(HoldingWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[HoldingWeapon->GetWeaponType()] -= ReloadAmount;
		CurrentCarriedAmmo = CarriedAmmoMap[HoldingWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ATFIPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CurrentCarriedAmmo);
	}
	HoldingWeapon->AddAmmo(ReloadAmount);
}

void UTFICombatComponent::UpdateCarriedValue()
{
	if (Character == nullptr || HoldingWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(HoldingWeapon->GetWeaponType()))
	{
		CurrentCarriedAmmo = CarriedAmmoMap[HoldingWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ATFIPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CurrentCarriedAmmo);
	}
}

void UTFICombatComponent::Slide()
{
	if (bSliding || bBulletJumping) return;
	bSliding = true;
	CharacterState = ECharacterState::ECS_Slide;
	ServerSlide();
}

void UTFICombatComponent::ServerSlide_Implementation()
{
	MultiCastSlide();
}

void UTFICombatComponent::MultiCastSlide_Implementation()
{
	InitSlide();
	SlideTrack.BindDynamic(this, &UTFICombatComponent::UpdateSlideSpeed);
	SlideTimeline = SlideTimeline == nullptr ? Character->GetSlideTimeline() : SlideTimeline;
	if (SlideTimeline && SlideCurve)
	{
		SlideTimeline->AddInterpFloat(SlideCurve, SlideTrack);
		SlideTimeline->PlayFromStart();
	}
	Character->PlaySlideMontage();
}

void UTFICombatComponent::UpdateSlideSpeed(float SpeedMultiple)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	FVector ForwardVector = Character->GetActorForwardVector();
	FVector Velocity = Character->GetVelocity();
	float MoveDistance = SlideStartSpeed * SlideSpeedMultiple * SpeedMultiple;
	FVector FinalVector = ForwardVector * MoveDistance;
	Character->GetCharacterMovement()->Velocity = FVector(FinalVector.X, FinalVector.Y, Velocity.Z);
}

void UTFICombatComponent::InitSlide()
{
	FVector SpeedVelocity = Character->GetVelocity();
	SpeedVelocity.Z = 0.f;
	SlideStartSpeed = FMath::Min(SpeedVelocity.Size(), Character->GetCharacterMovement()->MaxWalkSpeed * 2);
}

void UTFICombatComponent::SlideCompleted()
{
	if (SlideTimeline != nullptr) SlideTimeline->Stop();
	bSliding = false;
	CharacterState = ECharacterState::ECS_Normal;
	if (Character->GetCrouchButtonPressed() && !Character->GetCharacterMovement()->IsFalling())
	{
		Character->Crouch();
	}
	else
	{
		Character->UnCrouch();
	}
}

void UTFICombatComponent::BulletJump()
{
	if (bBulletJumping || CurrentBulletJumpLimit <= 0) return;
	if (SlideTimeline != nullptr) SlideTimeline->Stop();
	bSliding = false;
	bBulletJumping = true;
	CharacterState = ECharacterState::ECS_BulletJump;

	ReduceBulletJumpLimit(1);
	InitBulletJump();	// 在本地计算旋转是没意义的
	BulletJumpTrack.BindDynamic(this, &UTFICombatComponent::UpdateBulletJumpSpeed);
	BulletJumpTimeline = BulletJumpTimeline == nullptr ? Character->GetBulletJumpTimeline() : BulletJumpTimeline;
	if (BulletJumpTimeline && BulletJumpCurve)
	{
		BulletJumpTimeline->AddInterpFloat(BulletJumpCurve, BulletJumpTrack);
		BulletJumpTimeline->PlayFromStart();
	}
	ServerBulletJump();
}

void UTFICombatComponent::ServerBulletJump_Implementation()
{
	if (SlideTimeline != nullptr) SlideTimeline->Stop();
	BulletJumpTrack.BindDynamic(this, &UTFICombatComponent::UpdateBulletJumpSpeed);
	InitBulletJump();
	BulletJumpTimeline = BulletJumpTimeline == nullptr ? Character->GetBulletJumpTimeline() : BulletJumpTimeline;
	if (BulletJumpTimeline && BulletJumpCurve)
	{
		BulletJumpTimeline->AddInterpFloat(BulletJumpCurve, BulletJumpTrack);
		BulletJumpTimeline->PlayFromStart();
	}

	MulticastBulletJump();
}

void UTFICombatComponent::MulticastBulletJump_Implementation()
{
	Character->PlayBulletJumpMontage();
}

void UTFICombatComponent::UpdateBulletJumpSpeed(float CurveSpeedMultiple)
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return;
	float MoveDistance = BulletJumpDistance * CurveSpeedMultiple;
	FVector FinalVector = BulletJumpFacingVector * MoveDistance;
	Character->GetCharacterMovement()->Velocity = FinalVector;
	Character->SetActorRotation(BulletJumpFacingRotator);
}

void UTFICombatComponent::InitBulletJump()
{
	BulletJumpFacingRotator = Character->GetBaseAimRotation();
	BulletJumpFacingVector = BulletJumpFacingRotator.Vector();
	if (Character->GetCharacterMovement()->IsFalling())
	{
		BulletJumpFacingRotator.Pitch = BulletJumpFacingRotator.Pitch - 90.f;
	}
	else
	{
		BulletJumpFacingRotator.Pitch = FMath::Max(BulletJumpFacingRotator.Pitch - 90.f, -60.f);
		FVector JumpImpulse = FVector(0.f, 0.f, Character->GetCharacterMovement()->JumpZVelocity);
		Character->GetCharacterMovement()->AddImpulse(JumpImpulse, true);
		if (BulletJumpFacingVector.Z < 0.f)
		{
			BulletJumpFacingVector.Z = 0.f;
		}
	}
}

void UTFICombatComponent::OnRep_CharacterState()
{
	switch (CharacterState)
	{
	case ECharacterState::ECS_Reload:
		if (Character != nullptr && !Character->IsLocallyControlled()) LocalReload();
		break;
	case ECharacterState::ECS_Normal:
		if (bFireButtonPressed) Fire();
		break;
	default:
		return;
	}
}

bool UTFICombatComponent::CanFire()
{
	if (HoldingWeapon == nullptr) return false;
	return !HoldingWeapon->IsEmpty() && bCanFire && CharacterState == ECharacterState::ECS_Normal;
}

void UTFICombatComponent::BulletJumpCompleted()
{
	if (BulletJumpTimeline) BulletJumpTimeline->Stop();
	FRotator FinishedRotator = Character->GetBaseAimRotation();
	FinishedRotator.Pitch = 0.f;
	FinishedRotator.Roll = 0.f;
	Character->SetActorRotation(FinishedRotator);

	bBulletJumping = false;
	CharacterState = ECharacterState::ECS_Normal;
	Character->UnCrouch();
	if (!Character->GetCharacterMovement()->IsFalling())
	{
		ResetBulletJumpLimit();
	}

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

void UTFICombatComponent::SetFiring(bool state)
{
	bFireButtonPressed = state;
	if (bFireButtonPressed)
	{
		Fire();
	}
}

void UTFICombatComponent::SetDashButtonPressed(bool state)
{
	bDashButtonPressed = state;
	Dash(bDashButtonPressed);
}

void UTFICombatComponent::ReduceBulletJumpLimit(int32 ReduceAmount)
{
	CurrentBulletJumpLimit = FMath::Max(CurrentBulletJumpLimit - ReduceAmount, 0);
}

void UTFICombatComponent::ResetBulletJumpLimit()
{
	CurrentBulletJumpLimit = BulletJumpLimit;
}

float UTFICombatComponent::GetSlideStartSpeed()
{
	return SlideStartSpeed;
}