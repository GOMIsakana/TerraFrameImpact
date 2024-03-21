// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TerraFrameImpact/Enums/CharacterState.h"
#include "TerraFrameImpact/Enums/WeaponType.h"
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
	virtual void Landed(const FHitResult& Hit) override;
	UFUNCTION()
	void PlaySlideMontage();
	UFUNCTION()
	void PlayBulletJumpMontage();
	UFUNCTION()
	void PlayFireMontage(bool bAiming);
	UFUNCTION()
	void PlayReloadMontage(EWeaponType PlaySession);
	UFUNCTION()
	void PlayHitReactMontage();

	UFUNCTION()
	void Elim();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTFICombatComponent* CombatComponent;

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

	UFUNCTION()
	void OnFireButtonPressed();
	UFUNCTION()
	void OnFireButtonReleased();

	UFUNCTION()
	void OnReloadButtonPressed();

	UPROPERTY(EditAnywhere, Category = "角色动画")
	class UAnimMontage* SlideMontage;
	UPROPERTY(EditAnywhere, Category = "角色动画")
	UAnimMontage* BulletJumpMontage;
	UPROPERTY(EditAnywhere, Category = "角色动画")
	UAnimMontage* FireMontage;
	UPROPERTY(EditAnywhere, Category = "角色动画")
	UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category = "角色动画")
	UAnimMontage* HitReactMontage;

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch(float DeltaTime);
	float CalculateSpeed(); 
	
	UFUNCTION()
	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Server, Reliable)
	void ServerElim();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	UFUNCTION()
	virtual void KnockDown();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastKnockDown();

private:	

	UPROPERTY()
	class ATFIPlayerController* TFIPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

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
	bool bAO_YawOutofRange = false;
	bool bAO_PitchOutofRange = false;

	bool bShouldRotateRootBone = false;

	UPROPERTY(VisibleAnywhere)
	bool bCrouchButtonPressed = false;

	class UTimelineComponent* SlideTimeline;
	UTimelineComponent* BulletJumpTimeline;

	/*
	* 生命值护盾相关
	*/

	UPROPERTY(EditAnywhere, Category = "角色状态")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "角色状态", ReplicatedUsing = OnRep_Health)
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "角色状态")
	float MaxShield = 100.f;
	UPROPERTY(EditAnywhere, Category = "角色状态", ReplicatedUsing = OnRep_Shield)
	float Shield = 100.f;
	UPROPERTY(EditAnywhere, Category = "角色状态")
	float ShieldRecoveryDelay = 5.f;
	UPROPERTY(EditAnywhere, Category = "角色状态")
	float ShieldRecoveryAmountPerTick = 1.f;
	FTimerHandle ShieldRecoveryTimer;
	UPROPERTY(Replicated = true)
	bool bCanRecoveryShield = true;
	UFUNCTION()
	void OnShieldRecoveryTimerComplete();
	UFUNCTION()
	void RecoveryShield();

	UFUNCTION()
	void OnRep_Health(float LastHealth);
	UFUNCTION()
	void OnRep_Shield(float LastShield);

	UFUNCTION()
	void UpdateHUDHealth();
	UFUNCTION()
	void UpdateHUDShield();

	FTimerHandle ElimTimer;

	void OnElimTimerFinished();
	class ATFIGameMode* TFIGameMode;

	bool bElimmed = false;
	bool bDying = false;
	UPROPERTY(EditAnywhere, Category = "角色状态")
	float ElimDelay = 2.f;

public:

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE void SetAO_Yaw(float NewYaw) {  AO_Yaw = NewYaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	FORCEINLINE void SetStartingAimRotation(FRotator NewStartingAimRotation) { StartingAimRotation = NewStartingAimRotation; }
	FORCEINLINE bool GetAO_YawOutofRange() const { return bAO_YawOutofRange; }
	FORCEINLINE bool GetAO_PitchOutofRange() const { return bAO_PitchOutofRange; }
	FORCEINLINE bool GetShouldRotateRootBone() const { return bShouldRotateRootBone; }
	FORCEINLINE bool GetCrouchButtonPressed() const { return bCrouchButtonPressed; }
	FORCEINLINE UTimelineComponent* GetSlideTimeline() const { return SlideTimeline; }
	FORCEINLINE UTimelineComponent* GetBulletJumpTimeline() const { return BulletJumpTimeline; }
	AWeapon* GetHoldingWeapon();
	bool GetPreparingBattle();
	bool IsDashing();
	bool IsAiming();
	bool IsHoldingWeapon();
	ECharacterState GetCharacterState();
	FORCEINLINE UTFICombatComponent* GetCombatComponent() const { return CombatComponent; };
	FORCEINLINE bool IsDying() const { return bDying; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
};
