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

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	float CalculateSpeed();

private:	

	UPROPERTY()
	class USpringArmComponent* CameraBoom;

	UPROPERTY()
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

	/**
	* 瞄准偏移（给动画用的）
	*/
	UPROPERTY(VisibleAnywhere)
	float AO_Yaw;
	float InterpAO_Yaw;
	UPROPERTY(VisibleAnywhere)
	float AO_Pitch;
	FRotator StartingAimRotation;

	bool bShouldRotateRootBone = false;
	UPROPERTY(Replicated = true)
	bool bDashButtonPressed = false;


public:

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE bool GetShouldRotateRootBone() const { return bShouldRotateRootBone; }
	bool IsDashing();
	bool IsAiming();
	bool IsHoldingWeapon();
};
