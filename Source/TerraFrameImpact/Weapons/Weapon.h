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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
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

	UFUNCTION()
	void OnRep_WeaponState();

	void OnSetWeaponState();

	void OnHasOwner();
	void OnDropped();


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
