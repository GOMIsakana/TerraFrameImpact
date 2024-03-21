// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerraFrameImpact/Enums/WeaponState.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "TerraFrameImpact/Enums/WeaponType.h"
#include "TerraFrameImpact/Enums/FireType.h"
#include "Sound/SoundCue.h"
#include "Weapon.generated.h"

UCLASS()
class TERRAFRAMEIMPACT_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();

	UFUNCTION()
	void ShowPickupWidget(bool bVisible);
	UFUNCTION()
	void SetWeaponState(EWeaponState State);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Fire(const FVector& TargetPos);
	void SetHUDAmmo();
	void SetHUDCarriedAmmo();
	FVector TraceEndWithSpread(const FVector& HitTarget);

	UPROPERTY(EditAnywhere, Category = "武器属性")
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = "武器属性")
	bool bAutomatic = true;

	UPROPERTY(EditAnywhere, Category = "武器属性")
	bool bEnableSpread = true;

	UPROPERTY(EditAnywhere, Category = "准心")
	class UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = "准心")
	UTexture2D* CrosshairsBottom;
	UPROPERTY(EditAnywhere, Category = "准心")
	UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = "准心")
	UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = "准心")
	UTexture2D* CrosshairsCenter;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, Category = "武器基本资产", ReplicatedUsing = OnRep_WeaponState)
	EWeaponState WeaponState = EWeaponState::EWS_Unpicked;

	UPROPERTY(VisibleAnywhere, Category = "武器基本资产")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "武器基本资产")
	USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere, Category = "武器基本资产")
	class UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "武器基本资产")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "武器属性")
	EWeaponType WeaponType = EWeaponType::EWT_AssultRifle;

	UPROPERTY(EditAnywhere, Category = "武器属性")
	EFireType FireType;

	UPROPERTY(EditAnywhere, Category = "武器属性")
	float SpreadSphereRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = "武器属性")
	float SpreadSphereDistance = 800.f;

	UPROPERTY(EditAnywhere, Category = "武器属性")
	float Damage = 20.f;

private:

	UFUNCTION()
	void OnRep_WeaponState();
	void OnSetWeaponState();
	void OnHasOwner();
	void OnDropped();
	
	UPROPERTY(EditAnywhere)
	int32 Ammo;
	UPROPERTY(EditAnywhere)
	int32 CarriedAmmo;
	UPROPERTY(EditAnywhere)
	int32 MagCapacity;
	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo;

	UFUNCTION(Client, Reliable)
	void ClientUpdateAmmo(int32 ServerAmmo);
	UFUNCTION(Client, Reliable)
	void ClientAddAmmo(int32 AmmoToAdd);
	UFUNCTION(Client, Reliable)
	void ClientUpdateCarriedAmmo(int32 ServerAmmo);
	UFUNCTION(Client, Reliable)
	void ClientAddCarriedAmmo(int32 AmmoToAdd);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USkeletalMeshComponent* GetWeaponMesh();

	void SpendRound();
	void AddAmmo(int32 AmountToAdd);
	void AddCarriedAmmo(int32 AmountToAdd);

	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; };
	FORCEINLINE bool IsFull() const { return Ammo >= MagCapacity; };
	FORCEINLINE int32 GetAmmo() const { return Ammo; };
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; };
	FORCEINLINE int32 GetCarriedAmmo() const { return CarriedAmmo; };
	FORCEINLINE int32 GetMaxCarriedAmmo() const { return MaxCarriedAmmo; };
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; };
	FORCEINLINE EFireType GetFireType() const { return FireType; };
};
