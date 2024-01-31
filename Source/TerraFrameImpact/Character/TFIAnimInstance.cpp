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

	FRotator AimRotation = Character->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, 5.f);
	YawOffset = DeltaRotation.Yaw;

	AO_Yaw = Character->GetAO_Yaw();
	AO_Pitch = Character->GetAO_Pitch();
}