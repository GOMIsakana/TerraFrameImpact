// Fill out your copyright notice in the Description page of Project Settings.


#include "TFICharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "TerraFrameImpact/Components/TFICombatComponent.h"
#include "TerraFrameImpact/Weapons/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "TerraFrameImpact/TerraFrameImpact.h"
#include "TerraFrameImpact/Enums/WeaponType.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TerraFrameImpact/HUD/OverHeadHealthBar.h"

ATFICharacter::ATFICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("摄像机弹簧臂"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("摄像机"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	CombatComponent = CreateDefaultSubobject<UTFICombatComponent>(TEXT("战斗组件"));
	CombatComponent->SetIsReplicated(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("头顶血条"));
	HealthBarWidget->SetupAttachment(RootComponent);

	SlideTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("滑行时间轴"));
	BulletJumpTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("子弹跳时间轴"));

	GetCharacterMovement()->SetIsReplicated(true);
}

void ATFICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (CombatComponent != nullptr)
	{
		CombatComponent->SetDashButtonPressed(false);
		GetCharacterMovement()->MaxWalkSpeed = CombatComponent->WalkSpeed;
		GetCharacterMovement()->MaxWalkSpeedCrouched = CombatComponent->CrouchSpeed;
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ATFICharacter::ReceiveDamage);
	}
}

void ATFICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);

	if (bCanRecoveryShield)
	{
		RecoveryShield();
	}
	UpdateHUDHealth();
	UpdateHUDShield();
	/*
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	UE_LOG(LogTemp, Warning, TEXT("Walk Speed = %.3f"), Velocity.Size());
	*/
}

void ATFICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 100);
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATFICharacter::Move);
		}
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATFICharacter::Look);
		}
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATFICharacter::Jump);
		}
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &ATFICharacter::OnAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &ATFICharacter::OnAimCompoleted);
		}
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ATFICharacter::OnInteract);
			// EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATFICharacter::OnRequestElim);
		}
		if (DashAction)
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ATFICharacter::OnDashButtonPressed);
			// EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Canceled, this, &ATFICharacter::OnEndDash);
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ATFICharacter::OnDashButtonReleased);
		}
		if (CrouchAction)
		{
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ATFICharacter::OnCrouchButtonPressed);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ATFICharacter::OnCrouchButtonReleased);
		}
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ATFICharacter::OnFireButtonPressed);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ATFICharacter::OnFireButtonReleased);
		}
		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ATFICharacter::OnReloadButtonPressed);
		}
	}
	//PlayerInputComponent->BindAction(FName("Move"), ETriggerEvent::Triggered, this, );
	//UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	/*if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction();
	}*/

	//PlayerInputComponent->BindAction()
}

void ATFICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 条件为OwnerOnly时，只会对本地的客户端进行网络复制，不会影响到服务器（不太懂，总之这样写吧）
	DOREPLIFETIME_CONDITION(ATFICharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ATFICharacter, OverlappingInteractActor, COND_OwnerOnly);
	DOREPLIFETIME(ATFICharacter, Health);
	DOREPLIFETIME(ATFICharacter, Shield);
	DOREPLIFETIME(ATFICharacter, bCanRecoveryShield);
}

void ATFICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
}

void ATFICharacter::OnShieldRecoveryTimerComplete()
{
	bCanRecoveryShield = true;
}

void ATFICharacter::RecoveryShield()
{
	if (bDying || bElimmed) return;
	Shield = FMath::Clamp(Shield + ShieldRecoveryAmountPerTick, 0, MaxShield);
}

void ATFICharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (LastHealth > Health)
	{
		PlayHitReactMontage();
	}
}

void ATFICharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
}

void ATFICharacter::UpdateHUDHealth()
{
	TFIPlayerController = TFIPlayerController == nullptr ? Cast<ATFIPlayerController>(Controller) : TFIPlayerController;
	if (TFIPlayerController != nullptr)
	{
		TFIPlayerController->SetHUDHealth(Health, MaxHealth);
	}
	if (HealthBarWidget)
	{
		UOverHeadHealthBar* OverHeadHealthBar = Cast<UOverHeadHealthBar>(HealthBarWidget->GetWidget());
		bool bHUDValid = OverHeadHealthBar != nullptr &&
			OverHeadHealthBar->HealthBar;
		if (bHUDValid)
		{
			OverHeadHealthBar->HealthBar->SetPercent(Health / MaxHealth);
		}
	}
}

void ATFICharacter::UpdateHUDShield()
{
	TFIPlayerController = TFIPlayerController == nullptr ? Cast<ATFIPlayerController>(Controller) : TFIPlayerController;
	if (TFIPlayerController != nullptr)
	{
		TFIPlayerController->SetHUDShield(Shield, MaxShield);
	}
	if (HealthBarWidget)
	{
		UOverHeadHealthBar* OverHeadHealthBar = Cast<UOverHeadHealthBar>(HealthBarWidget->GetWidget());
		bool bHUDValid = OverHeadHealthBar != nullptr &&
			OverHeadHealthBar->ShieldBar;
		if (bHUDValid)
		{
			OverHeadHealthBar->ShieldBar->SetPercent(Shield / MaxShield);
		}
	}
}

void ATFICharacter::OnElimTimerFinished()
{
	bElimmed = false;
	TFIGameMode = TFIGameMode == nullptr ? GetWorld()->GetAuthGameMode<ATFIGameMode>() : TFIGameMode;
	TFIPlayerController = TFIPlayerController == nullptr ? Cast<ATFIPlayerController>(Controller) : TFIPlayerController;
	if (TFIGameMode != nullptr && TFIPlayerController != nullptr)
	{
		TFIGameMode->RequestRespawn(this, TFIPlayerController);
	}
	bDying = false;
	bElimmed = false;
	Health = MaxHealth;
	Shield = MaxShield;
	UpdateHUDHealth();
	UpdateHUDShield();
}

AWeapon* ATFICharacter::GetHoldingWeapon()
{
	if (CombatComponent && CombatComponent->HoldingWeapon)
	{
		return CombatComponent->HoldingWeapon;
	}
	return nullptr;
}
bool ATFICharacter::GetPreparingBattle()
{
	return CombatComponent && CombatComponent->bPreparingBattle;
}
ECharacterState ATFICharacter::GetCharacterState()
{
	if (CombatComponent)
	{
		return CombatComponent->CharacterState;
	}
	return ECharacterState::ECS_MAX;
}

bool ATFICharacter::IsDashing()
{
	return (CombatComponent && CombatComponent->bIsDashing);
}

bool ATFICharacter::IsAiming()
{
	return (CombatComponent && CombatComponent->bIsAiming);
}

void ATFICharacter::Move(const FInputActionValue& Value)
{
	if (bDying || bElimmed) return;
	// 这里是增量的移动输入
	FVector2D Movement = Value.Get<FVector2D>();

	// 这里是获取现有的朝向与旋转
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		// 获取现有的平视方向的旋转（就是以角色头顶到脚为轴，进行左右旋转）
		const FRotator ControlRotation = PlayerController->GetControlRotation();
		const FRotator YawRotation = FRotator(0.f, ControlRotation.Yaw, 0.f);

		// 计算向前向量以及向右向量
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 添加输入，后面的参数是用来乘算的
		AddMovementInput(ForwardDirection, Movement.Y);
		AddMovementInput(RightDirection, Movement.X);
	}
}

void ATFICharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		AddControllerPitchInput(LookVector.X);
		AddControllerYawInput(LookVector.Y);
	}
}

void ATFICharacter::Jump()
{
	if (bDying || bElimmed) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	if (GetCharacterState() == ECharacterState::ECS_Slide)
	{
		CombatComponent->BulletJump();
		return;
	}
	Super::Jump();
}

void ATFICharacter::StopJumping()
{
	Super::StopJumping();
}

void ATFICharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (CombatComponent)
	{
		CombatComponent->ResetBulletJumpLimit();
	}
}

void ATFICharacter::OnAim()
{
	if (CombatComponent && CombatComponent->bIsAiming == false)
	{
		CombatComponent->SetAiming(true);
	}
}

void ATFICharacter::OnAimCompoleted()
{
	if (CombatComponent && CombatComponent->bIsAiming == true)
	{
		CombatComponent->SetAiming(false);
	}
}

void ATFICharacter::OnInteract()
{
	if (bElimmed) return;
	if (bDying)
	{
		Elim();
		return;
	}
	// if (bElimmed || bDying) return;
	ServerOnInteract();
}

void ATFICharacter::OnDashButtonPressed()
{
	if (bDying || bElimmed) return;
	if (GetCharacterState() != ECharacterState::ECS_Normal) return;
	if (bIsCrouched)
	{
		UnCrouch();
	}
	CombatComponent->SetDashButtonPressed(true);
}

void ATFICharacter::OnDashButtonReleased()
{
	if (CombatComponent == nullptr) return;
	CombatComponent->SetDashButtonPressed(false);
}

void ATFICharacter::OnCrouchButtonPressed()
{
	if (bDying || bElimmed) return;
	bCrouchButtonPressed = true;
	if ((GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f || GetCharacterMovement()->IsFalling()) && CombatComponent && GetCharacterState() == ECharacterState::ECS_Normal)
	{
		CombatComponent->Slide();
	}
	if (GetCharacterState() == ECharacterState::ECS_BulletJump) return;
	Crouch();
}

void ATFICharacter::OnCrouchButtonReleased()
{
	bCrouchButtonPressed = false;
	if (GetCharacterState() != ECharacterState::ECS_Normal) return;
	UnCrouch();
}

void ATFICharacter::OnFireButtonPressed()
{
	if (bDying || bElimmed) return;
	if (CombatComponent)
	{
		CombatComponent->SetFiring(true);
	}
}

void ATFICharacter::OnFireButtonReleased()
{
	if (CombatComponent)
	{
		CombatComponent->SetFiring(false);
	}
}

void ATFICharacter::OnReloadButtonPressed()
{
	if (bDying || bElimmed) return;
	if (CombatComponent)
	{
		CombatComponent->ReloadWeapon();
	}
}

void ATFICharacter::PlaySlideMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && SlideMontage != nullptr)
	{
		AnimInstance->Montage_Play(SlideMontage);
	}
}

void ATFICharacter::PlayBulletJumpMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && BulletJumpMontage != nullptr)
	{
		AnimInstance->Montage_Play(BulletJumpMontage);
	}
}

void ATFICharacter::PlayFireMontage(bool bAiming)
{
	if (CombatComponent == nullptr || CombatComponent->HoldingWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr || FireMontage == nullptr) return;
	AnimInstance->Montage_Play(FireMontage);
	if (bAiming)
	{
		AnimInstance->Montage_JumpToSection(FName("Aim"));
	}
	else
	{
		AnimInstance->Montage_JumpToSection(FName("Hip"));
	}
}

void ATFICharacter::PlayReloadMontage(EWeaponType PlaySession)
{
	if (CombatComponent == nullptr || CombatComponent->HoldingWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance == nullptr || ReloadMontage == nullptr) return;

	AnimInstance->Montage_Play(ReloadMontage);
	FName SessionName = FName("Rifle");
	switch (PlaySession)
	{
	case EWeaponType::EWT_AssultRifle:
		SessionName = FName("Rifle");
		break;
	}
	AnimInstance->Montage_JumpToSection(SessionName);
}

void ATFICharacter::PlayHitReactMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance != nullptr && BulletJumpMontage != nullptr)
	{
		AnimInstance->Montage_Play(HitReactMontage);
	}
}

void ATFICharacter::ServerOnInteract_Implementation()
{
	if (OverlappingWeapon != nullptr && CombatComponent != nullptr)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
	if (OverlappingInteractActor != nullptr)
	{
		OverlappingInteractActor->ServerInteract();
	}
}

void ATFICharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (bDying || bElimmed) return;
	if (IsLocallyControlled() && OverlappingWeapon != nullptr)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	// 这里的改变会调用OnRep函数，这个函数会仅在非服务器的客户端的本地上执行（就是 只有触发这个函数的客户端会去执行，服务端和其他客户端不会执行）
	OverlappingWeapon = Weapon;
	// 自身是某一个客户端，或者服务器本人才会将字显形（显形给自己，如果需要全部人显形需要用Server分发）（避免其他人触发重叠时让它的字显形）
	if (IsLocallyControlled() && OverlappingWeapon != nullptr)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
}

void ATFICharacter::SetOverlappingInteractActor(AInteractActor* InteractActor)
{
	if (bDying || bElimmed) return;
	if (IsLocallyControlled() && OverlappingInteractActor != nullptr)
	{
		OverlappingInteractActor->ShowInteractWidget(false);
	}
	// 这里的改变会调用OnRep函数，这个函数会仅在非服务器的客户端的本地上执行（就是 只有触发这个函数的客户端会去执行，服务端和其他客户端不会执行）
	OverlappingInteractActor = InteractActor;
	// 自身是某一个客户端，或者服务器本人才会将字显形（显形给自己，如果需要全部人显形需要用Server分发）（避免其他人触发重叠时让它的字显形）
	if (IsLocallyControlled() && OverlappingInteractActor != nullptr)
	{
		InteractActor->ShowInteractWidget(true);
	}
}

void ATFICharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon != nullptr)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon != nullptr)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void ATFICharacter::OnRep_OverlappingInteractActor(AInteractActor* LastInteractActor)
{
	if (OverlappingInteractActor != nullptr)
	{
		OverlappingInteractActor->ShowInteractWidget(true);
	}
	if (LastInteractActor != nullptr)
	{
		LastInteractActor->ShowInteractWidget(false);
	}
}

bool ATFICharacter::IsHoldingWeapon()
{
	return CombatComponent != nullptr && CombatComponent->HoldingWeapon != nullptr;
}

void ATFICharacter::AimOffset(float DeltaTime)
{
	if (bDying || bElimmed) return;
	if (GetCharacterMovement() == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();
	if (CombatComponent && Speed == 0.f && !bAO_YawOutofRange)
	{
		CombatComponent->ClearPreparingBattleTimer();
	}

	LastFrameAO_Yaw = AO_Yaw;		// 记录上一帧的AO_Yaw,以保证回正操作正常执行
	FRotator CurAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
	FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurAimRotation, StartingAimRotation);
	AO_Yaw = DeltaRotation.Yaw;
	// 不瞄准的时候才使用ASD控制角色方向
	if (CombatComponent && !(CombatComponent->bIsAiming || CombatComponent->bPreparingBattle))
	{
		// 这里是左右的(Yaw)
		if (Speed == 0.f && !bIsInAir)
		{
			// 对于超过扭头角度的，不做响应（例如玩家正在看角色正面），从当前的脑袋旋转逐步回正到正面
			if (AO_Yaw < -90.f || AO_Yaw > 90.f)
			{
				bAO_YawOutofRange = true;
				AO_Yaw = FMath::FInterpTo(LastFrameAO_Yaw, 0.f, DeltaTime, 3.f);
			}
			else
			{
				bAO_YawOutofRange = false;
				AO_Yaw = FMath::FInterpTo(LastFrameAO_Yaw, DeltaRotation.Yaw, DeltaTime, 3.f);
			}
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		if (Speed > 0.f || bIsInAir)
		{
			bAO_YawOutofRange = false;
			bUseControllerRotationYaw = false;
			if (bIsInAir)
			{
				GetCharacterMovement()->bOrientRotationToMovement = false;
			}
			else
			{
				GetCharacterMovement()->bOrientRotationToMovement = true;
			}
			// StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
			StartingAimRotation = FRotator(0.f, GetActorRotation().Yaw, 0.f);
			AO_Yaw = 0.f;
		}
	}
	else
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bAO_YawOutofRange = false;
	}
	// 这里是上下的(Pitch)
	CalculateAO_Pitch(DeltaTime);
}

void ATFICharacter::CalculateAO_Pitch(float DeltaTime)
{
	bAO_PitchOutofRange = false;
	LastFrameAO_Pitch = AO_Pitch;
	AO_Pitch = GetBaseAimRotation().Pitch;
	// 因为客机传输不会传负数，所以需要将正270~360度修正到-90~0度这个区间
	if (!IsLocallyControlled() && AO_Pitch > 90.f)
	{
		FVector2D CurrentRange(270.f, 360.f);
		FVector2D TargetRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(CurrentRange, TargetRange, AO_Pitch);
	}
	// 对于超过的角度，逐步回正到原来的位置
	if (AO_Pitch > 90.f || AO_Pitch < -90.f)
	{
		bAO_PitchOutofRange = true;
		AO_Pitch = FMath::FInterpTo(LastFrameAO_Pitch, 0.f, DeltaTime, 3.f);
	}
}

float ATFICharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}

void ATFICharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (bDying || bElimmed) return;
	if (Shield > 0.f)
	{
		Shield = FMath::Clamp(Shield - Damage, 0, MaxShield);
	}
	else if (Health > 0.f)
	{
		Health = FMath::Clamp(Health - Damage, 0, MaxHealth);
	}
	UpdateHUDHealth();
	UpdateHUDShield();
	bCanRecoveryShield = false;

	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(ShieldRecoveryTimer);
		World->GetTimerManager().SetTimer(
			ShieldRecoveryTimer,
			this,
			&ATFICharacter::OnShieldRecoveryTimerComplete,
			ShieldRecoveryDelay
		);
	}
	
	if (Health <= 0.f)
	{
		KnockDown();
	}
	
}

void ATFICharacter::Elim()
{
	if (bElimmed) return;
	SetHealthBarDisplay(false);
	bDying = false;
	bElimmed = true;
	TFIPlayerController = TFIPlayerController == nullptr ? Cast<ATFIPlayerController>(Controller) : TFIPlayerController;
	if (TFIPlayerController != nullptr)
	{
		TFIPlayerController->SetHUDRespawnNotify(ESlateVisibility::Hidden);
	}
	ServerElim();
}

void ATFICharacter::InteractWithCorsshairs()
{
	SetHealthBarDisplay(true);
}

void ATFICharacter::SetHealthBarDisplay(bool bDisplay)
{
	if (HealthBarWidget && !(bDying || bElimmed))
	{
		HealthBarWidget->SetVisibility(bDisplay);
		if (bDisplay)
		{
			GetWorld()->GetTimerManager().ClearTimer(HealthBarDisplayTimer);
			GetWorld()->GetTimerManager().SetTimer(
				HealthBarDisplayTimer,
				this,
				&ATFICharacter::OnHealthBarDisplayTimerFinished,
				HealthBarInvisibilityDelay
			);
		}
	}
}

void ATFICharacter::OnHealthBarDisplayTimerFinished()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void ATFICharacter::ServerElim_Implementation()
{
	MulticastElim();
}

void ATFICharacter::MulticastElim_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer(
		ElimTimer,
		this,
		&ATFICharacter::OnElimTimerFinished,
		ElimDelay
	);
}

void ATFICharacter::KnockDown()
{
	if (bDying || bElimmed) return;
	SetHealthBarDisplay(false);
	bDying = true;
	MulticastKnockDown();
}

void ATFICharacter::MulticastKnockDown_Implementation()
{
	if (CombatComponent && CombatComponent->HoldingWeapon != nullptr)
	{
		CombatComponent->DropWeapon();
	}
	bDying = true;
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);
	TFIPlayerController = TFIPlayerController == nullptr ? Cast<ATFIPlayerController>(Controller) : TFIPlayerController;
	if (TFIPlayerController != nullptr)
	{
		TFIPlayerController->SetHUDRespawnNotify(ESlateVisibility::Visible);
	}
	GetCharacterMovement()->DisableMovement();
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
