// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIAICharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "TerraFrameImpact/Components/TFICombatComponent.h"
#include "TerraFrameImpact/Weapons/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "TerraFrameImpact/TerraFrameImpact.h"
#include "TerraFrameImpact/Enums/WeaponType.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "TerraFrameImpact/PlayerState/TFIPlayerState.h"
#include "TerraFrameImpact/GameState/TFIGameState.h"

void ATFIAICharacter::BeginPlay()
{
	Super::BeginPlay();
	UWorld* World = GetWorld();
	if (World && CombatComponent && GetHoldingWeapon() == nullptr && HasAuthority())
	{
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->SetReplicates(true);
		CombatComponent->EquipWeapon(StartingWeapon);
	}
}

void ATFIAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFIAICharacter::KnockDown()
{
	Super::KnockDown();


	if (CombatComponent && GetHoldingWeapon() != nullptr)
	{
		CombatComponent->DropWeapon();
	}
	ATFIGameMode* GameMode = GetWorld()->GetAuthGameMode<ATFIGameMode>();
	if (GameMode != nullptr)
	{
		if (LastAttackerController != nullptr)
		{
			GameMode->PlayerElimEnemy(LastAttackerController, 1);
		}
		ATFIGameState* GameState = GameMode->GetGameState<ATFIGameState>();
		if (GameState != nullptr)
		{
			GameState->AddEnemyAmount(-1);
			GameState->AddTotalElimAmount(1);
		}
	}
}

void ATFIAICharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::ReceiveDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	LastAttackerController = Cast<ATFIPlayerController>(InstigatedBy);
}
