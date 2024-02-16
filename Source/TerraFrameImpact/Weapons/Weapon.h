// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerraFrameImpact/Enums/WeaponState.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
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

	UPROPERTY(EditAnywhere, Category = "武器基本资产")
	float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = "武器基本资产")
	bool bAutomatic = true;

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

private:

	UFUNCTION()
	void OnRep_WeaponState();
	void OnSetWeaponState();
	void OnHasOwner();
	void OnDropped();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	USkeletalMeshComponent* GetWeaponMesh();

	UPROPERTY(EditAnywhere)
	int32 Ammo;

	void SpendRound();

	UPROPERTY(EditAnywhere)
	int32 MagCapacity;

	FORCEINLINE bool IsEmpty() const { return Ammo <= 0; };
};
