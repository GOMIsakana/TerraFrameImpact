// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWeapon::AWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("武器网格体"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("拾取碰撞"));
	Sphere->SetupAttachment(RootComponent);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("拾取提示"));
	PickupWidget->SetupAttachment(RootComponent);

}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWeapon, WeaponState);
}

void AWeapon::Fire(const FVector& TargetPos)
{
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	SpendRound();
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	// 只有在主机才会附加重叠与查询操作，避免客户端本地触发后发送到服务端，然后服务端经过复制影响到其他客户端
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereStartOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);
	}

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}

void AWeapon::OnSphereStartOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 联系到玩家类, 让它去显示这个HUD，并加入到这个玩家的拾取对象中
	ATFICharacter* Character = Cast<ATFICharacter>(OtherActor);
	if (Character == nullptr) return;
	Character->SetOverlappingWeapon(this);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 联系到玩家类, 让它去不显示这个HUD，并从这个玩家的拾取对象中移除
	ATFICharacter* Character = Cast<ATFICharacter>(OtherActor);
	if (Character == nullptr) return;
	Character->SetOverlappingWeapon(nullptr);	// 角色那边收到nullptr会自动取消重叠
}

void AWeapon::ShowPickupWidget(bool bVisible)
{
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bVisible);
	}
}

void AWeapon::SetWeaponState(EWeaponState State)
{
	WeaponState = State;
	OnSetWeaponState();
}

void AWeapon::OnRep_WeaponState()
{
	OnSetWeaponState();
}

void AWeapon::OnSetWeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_HasOwner:
		OnHasOwner();
		break;
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	default:
		return;
	}
}

void AWeapon::OnHasOwner()
{
	ShowPickupWidget(false);
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::OnDropped()
{
	// 设置主人的操作在战斗类里面实现
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority) return;
	Ammo = ServerAmmo;
	SetHUDAmmo();
}

void AWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	if (GetLocalRole() == ENetRole::ROLE_Authority) return;

	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);
	ATFICharacter* Character = Cast<ATFICharacter>(GetOwner());
	SetHUDAmmo();
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

USkeletalMeshComponent* AWeapon::GetWeaponMesh()
{
	if (WeaponMesh != nullptr)
	{
		return WeaponMesh;
	}
	return nullptr;
}

void AWeapon::SetHUDAmmo()
{
	ATFICharacter* Character = Cast<ATFICharacter>(GetOwner());
	if (Character)
	{
		ATFIPlayerController* Controller = Cast<ATFIPlayerController>(Character->Controller);
		if (Controller)
		{
			Controller->SetHUDWeaponAmmo(Ammo);
		}
	}
}

void AWeapon::SpendRound()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
	SetHUDAmmo();
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		ClientUpdateAmmo(Ammo);
	}
}

void AWeapon::AddAmmo(int32 AmountToAdd)
{
	Ammo = FMath::Clamp(Ammo + AmountToAdd, 0, MagCapacity);
	SetHUDAmmo();
	ClientAddAmmo(AmountToAdd);
}
