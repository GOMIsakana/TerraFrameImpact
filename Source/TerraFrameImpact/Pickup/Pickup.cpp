// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("物品模型"));
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	PickupMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	PickupMesh->SetSimulatePhysics(true);

	SetRootComponent(PickupMesh);

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("碰撞箱"));
	PickupSphere->SetupAttachment(RootComponent);
	PickupSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn ,ECollisionResponse::ECR_Overlap);
}

void APickup::Destroyed()
{
	Super::Destroyed();

	if (PickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
	if (PickupEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			PickupEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(
			BindTimer,
			this,
			&APickup::OnBindTimerComplete,
			BindOverlapDelay
		);
	}
	
}

void APickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void APickup::OnBindTimerComplete()
{
	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnBeginOverlap);
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

