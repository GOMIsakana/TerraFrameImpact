// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "Components/TimelineComponent.h"
#include "TerraFrameImpact/Enums/CharacterState.h"
#include "TerraFrameImpact/HUD/TFICharacterHUD.h"
#include "TFICombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TERRAFRAMEIMPACT_API UTFICombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	friend class ATFICharacter;
	// Sets default values for this component's properties
	UTFICombatComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetAiming(bool state);
	void SetFiring(bool state);
	void SetDashButtonPressed(bool state);
	void ReduceBulletJumpLimit(int32 ReduceAmount);
	void ResetBulletJumpLimit();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_IsAiming();

	void SetHUDCrosshairs(float DeltaTime);

private:
	UFUNCTION()
	void AttachActorToCharacterMesh(AActor* OtherActor, FName SocketName);

	UPROPERTY(VisibleAnywhere)
	ATFICharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_HoldingWeapon)
	class AWeapon* HoldingWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_IsAiming)
	bool bIsAiming = false;
	bool bAimButtonPressed = false;
	UPROPERTY(Replicated = true)
	bool bIsDashing = false;
	bool bDashButtonPressed = false;

	UFUNCTION()
	void EquipWeapon(AWeapon* TargetWeapon);

	UFUNCTION()
	void DropWeapon();

	UFUNCTION()
	void OnRep_HoldingWeapon();

	UFUNCTION(Server, Reliable)
	void ServerStartDash();
	UFUNCTION()
	void Dash(bool state);
	UFUNCTION(Server, Reliable)
	void ServerStopDash();

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool state);

	void TraceUnderCrossHair(FHitResult& HitResult);
	void Fire();
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TargetPos);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TargetPos);
	void LocalFire(const FVector_NetQuantize& TargetPos);
	FVector HitTarget;
	FTimerHandle FireTimer;
	UFUNCTION()
	void StartFireTimer();
	UFUNCTION()
	void FireTimerFinished();
	bool bCanFire = true;
	bool bFireButtonPressed;


	UFUNCTION()
	void Slide();
	UFUNCTION(Server, Reliable)
	void ServerSlide();
	UFUNCTION(NetMultiCast, Reliable)
	void MultiCastSlide();
	UFUNCTION()
	void UpdateSlideSpeed(float CurveSpeedMultiple);
	UFUNCTION(BlueprintCallable)
	void SlideCompleted();
	UFUNCTION()
	void InitSlide();
	bool bSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float SlideSpeedMultiple = 5.f;
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	// float SlideZAixSpeedMultiple = 2.f;
	class FOnTimelineFloat SlideTrack;	// 用来跟踪Timeline的输出, 实际上就是负责将Timeline的输出进行处理的
	UPROPERTY(EditAnywhere, Category = "Move")
	class UCurveFloat* SlideCurve;
	UTimelineComponent* SlideTimeline;
	float SlideStartSpeed = 0.f;

	UFUNCTION()
	void BulletJump();
	UFUNCTION()
	void UpdateBulletJumpSpeed(float CurveSpeedMultiple);
	UFUNCTION(Server, Reliable)
	void ServerBulletJump();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastBulletJump();
	UFUNCTION(BlueprintCallable)
	void BulletJumpCompleted();
	UFUNCTION()
	void InitBulletJump();
	bool bBulletJumping = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float BulletJumpDistance = 3000.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	int32 BulletJumpLimit = 1;
	UPROPERTY(EditAnywhere, Category = "Move")
	UCurveFloat* BulletJumpCurve;
	FOnTimelineFloat BulletJumpTrack;
	UTimelineComponent* BulletJumpTimeline;
	UPROPERTY(Replicated = true)
	FRotator BulletJumpFacingRotator;
	FVector BulletJumpFacingVector;
	int32 CurrentBulletJumpLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float DashSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float AimingSpeed = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float CrouchSpeed = 200.f;

	UPROPERTY(Replicated = true)
	ECharacterState CharacterState = ECharacterState::ECS_Normal;

	UPROPERTY()
	class ATFIPlayerController* Controller;
	UPROPERTY()
	class ATFICharacterHUD* HUD;
	FCrosshairPackage HUDPackage;
	float CrosshairVelocityFactor;
	float CrosshairFallingFactor;
	float CrosshairAimFactor;
	float CrosshairShootFactor;

	bool CanFire();

public:	
	UFUNCTION(BlueprintCallable)
	float GetSlideStartSpeed();
};
