// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
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
