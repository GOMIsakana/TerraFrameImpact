// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIAnimInstance.h"
#include "TFICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UTFIAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<ATFICharacter>(TryGetPawnOwner());
}

void UTFIAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (Character == nullptr)
	{
		Character = Cast<ATFICharacter>(TryGetPawnOwner());
	}
	if (Character == nullptr)
	{
		return;
	}

	FVector Velocity = Character->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = Character->GetCharacterMovement()->IsFalling();
	bIsAccelerating = Character->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bIsCrouched = Character->bIsCrouched;
	bIsAiming = Character->IsAiming();
	bIsDashing = Character->IsDashing();
	bIsHoldingWeapon = Character->IsHoldingWeapon();
	bCrouchButtonPressed = Character->GetCrouchButtonPressed();

	// 锁方向的旋转计算，即角色移动方向距离当前瞄准方向的差值
	FRotator AimRotator = Character->GetBaseAimRotation();
	FRotator MovementRotator = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
	FRotator InterpRotator = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotator, AimRotator);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, InterpRotator, DeltaSeconds, 6.f);
	YawOffset = DeltaRotation.Yaw;

	// 原地的瞄准
	AO_Yaw = Character->GetAO_Yaw();
	AO_Pitch = Character->GetAO_Pitch();
}