// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIPlayerController.h"
#include "TerraFrameImpact/HUD/TFICharacterHUD.h"
#include "TerraFrameImpact/HUD/CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

void ATFIPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInit();
}

void ATFIPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		TFICharacterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

void ATFIPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		TFICharacterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void ATFIPlayerController::SetHUDHealth(float CurHealth, float MaxHealth)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->HealthBar &&
		TFICharacterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		TFICharacterHUD->CharacterOverlay->HealthBar->SetPercent(CurHealth / MaxHealth);
		FString HealthText = FString::Printf(TEXT("%.0f"), CurHealth);
		TFICharacterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}

}

void ATFIPlayerController::SetHUDShield(float CurShield, float MaxShield)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->ShieldBar &&
		TFICharacterHUD->CharacterOverlay->ShieldText;
	if (bHUDValid)
	{
		TFICharacterHUD->CharacterOverlay->ShieldBar->SetPercent(CurShield / MaxShield);
		FString ShieldText = FString::Printf(TEXT("%.0f"), CurShield);
		TFICharacterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
}

void ATFIPlayerController::SetHUDRespawnNotify(ESlateVisibility State)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->RespawnNotify;
	if (bHUDValid)
	{
		TFICharacterHUD->CharacterOverlay->RespawnNotify->SetVisibility(State);
	}
}

void ATFIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TFICharacterHUD = Cast<ATFICharacterHUD>(GetHUD());
	if (TFICharacterHUD != nullptr)
	{
		TFICharacterHUD->AddCharacterOverlay();
	}
	SetHUDRespawnNotify(ESlateVisibility::Hidden);
}

void ATFIPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (TFICharacterHUD && TFICharacterHUD->CharacterOverlay)
		{
			CharacterOverlay = TFICharacterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);

			}
		}
	}
}
