// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TFICharacter.generated.h"

UCLASS()
class TERRAFRAMEIMPACT_API ATFICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ATFICharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void SetOverlappingWeapon(AWeapon* Weapon);

	virtual void Jump() override;
	virtual void StopJumping() override;
	UFUNCTION()
	void PlaySlideMontage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void OnAim();

	UFUNCTION()
	void OnAimCompoleted();

	UFUNCTION()
	void OnInteract();

	UFUNCTION(Server, Reliable)
	void ServerOnInteract();

	UFUNCTION()
	void OnDashButtonPressed();

	UFUNCTION()
	void OnDashButtonReleased();

	UFUNCTION()
	void OnCrouchButtonPressed();

	UFUNCTION()
	void OnCrouchButtonReleased();

	UPROPERTY(EditAnywhere, Category = "角色动画")
	class UAnimMontage* SlideMontage;

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch(float DeltaTime);
	float CalculateSpeed();

private:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTFICombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/**
	* 增强输入
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/**
	* 瞄准偏移（给动画用的）
	*/
	UPROPERTY(VisibleAnywhere)
	float AO_Yaw;
	float LastFrameAO_Yaw;
	UPROPERTY(VisibleAnywhere)
	float AO_Pitch;
	float LastFrameAO_Pitch;
	FRotator StartingAimRotation;

	bool bShouldRotateRootBone = false;

	UPROPERTY(VisibleAnywhere)
	bool bCrouchButtonPressed = false;

	UPROPERTY(VisibleAnywhere)
	class UTimelineComponent* SlideTimeline;

public:

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE bool GetShouldRotateRootBone() const { return bShouldRotateRootBone; }
	FORCEINLINE bool GetCrouchButtonPressed() const { return bCrouchButtonPressed; }
	FORCEINLINE UTimelineComponent* GetSlideTimeline() const { return SlideTimeline; }
	bool IsDashing();
	bool IsAiming();
	bool IsHoldingWeapon();
};
