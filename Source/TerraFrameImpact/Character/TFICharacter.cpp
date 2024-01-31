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

ATFICharacter::ATFICharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("摄像机弹簧臂"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("摄像机"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	CombatComponent = CreateDefaultSubobject<UTFICombatComponent>(TEXT("战斗组件"));
	CombatComponent->SetIsReplicated(true);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ATFICharacter::BeginPlay()
{
	Super::BeginPlay();

	CombatComponent->SetDashButtonPressed(false);
}

void ATFICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
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
		}
		if (DashAction)
		{
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ATFICharacter::OnDashButtonPressed);
			// EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Canceled, this, &ATFICharacter::OnEndDash);
			EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ATFICharacter::OnDashButtonReleased);
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
}

void ATFICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
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
	if (GetCharacterMovement() && GetCharacterMovement()->IsFalling()) return;
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
	Super::Jump();
}

void ATFICharacter::StopJumping()
{
	Super::StopJumping();
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
	ServerOnInteract();
}

void ATFICharacter::OnDashButtonPressed()
{
	if (CombatComponent == nullptr) return;
	CombatComponent->SetDashButtonPressed(true);
}

void ATFICharacter::OnDashButtonReleased()
{
	if (CombatComponent == nullptr) return;
	CombatComponent->SetDashButtonPressed(false);
}

void ATFICharacter::ServerOnInteract_Implementation()
{
	if (OverlappingWeapon != nullptr && CombatComponent != nullptr)
	{
		CombatComponent->EquipWeapon(OverlappingWeapon);
	}
}

void ATFICharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
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

bool ATFICharacter::IsHoldingWeapon()
{
	return CombatComponent != nullptr && CombatComponent->HoldingWeapon != nullptr;
}

void ATFICharacter::AimOffset(float DeltaTime)
{
	if (GetCharacterMovement() == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// 不瞄准的时候才使用ASD控制角色方向
	if (CombatComponent && !CombatComponent->bIsAiming)
	{
		// 这里是左右的(Yaw)
		if (Speed == 0.f && !bIsInAir)
		{
			FRotator CurAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
			FRotator DeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurAimRotation, StartingAimRotation);
			AO_Yaw = DeltaRotation.Yaw;
			// 对于超过扭头角度的，不做响应（例如玩家正在看角色正面），从当前的脑袋旋转逐步回正到正面
			if (AO_Yaw < -90.f || AO_Yaw > 90.f)
			{
				AO_Yaw = 0.f;
			}
			bUseControllerRotationYaw = false;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		if (Speed > 0.f || bIsInAir)
		{
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
		bUseControllerRotationYaw = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	// 这里是上下的(Pitch)
	CalculateAO_Pitch();
}

void ATFICharacter::CalculateAO_Pitch()
{
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
		AO_Pitch = 0.f;
	}
}

float ATFICharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	return Velocity.Size();
}
