// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "Components/TimelineComponent.h"
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
	void SetDashButtonPressed(bool state);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_IsAiming();

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

	UFUNCTION(Server, Reliable)
	void ServerSlide();
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerSlideCompleted();
	UFUNCTION(NetMultiCast, Reliable)
	void MultiCastSlide();
	UFUNCTION(NetMultiCast, Reliable)
	void MultiCastSlideComplete();
	UFUNCTION()
	void Slide();
	float bSliding = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float SlideSpeedMultiple = 1.f;
	UFUNCTION()
	void UpdateSlideSpeed(float SpeedMultiple);
	class FOnTimelineFloat SlideTarck;	// 用来跟踪Timeline的输出, 实际上就是负责将Timeline的输出进行处理的
	UPROPERTY(EditAnywhere, Category = "Move")
	class UCurveFloat* SlideCurve;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 300.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float DashSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float AimingSpeed = 200.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move", meta = (AllowPrivateAccess = "true"))
	float CrouchSpeed = 200.f;

public:	
	
};
