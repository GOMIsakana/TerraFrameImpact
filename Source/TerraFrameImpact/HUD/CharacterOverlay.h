// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/VerticalBox.h"
#include "CharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API UCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WeaponAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RespawnNotify;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimer;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* ScoreBoard;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* MissionBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MissionText;
};
