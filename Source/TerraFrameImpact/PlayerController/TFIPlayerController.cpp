// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIPlayerController.h"
#include "TerraFrameImpact/HUD/TFICharacterHUD.h"
#include "TerraFrameImpact/HUD/CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TerraFrameImpact/PlayerState/TFIPlayerState.h"
#include "TerraFrameImpact/GameState/TFIGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ATFIPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDGameTimer(GetServerTime());
	CheckTimeSync(DeltaTime);
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

void ATFIPlayerController::SetHUDScoreBoard(const FString& OtherPlayerName, int32 OtherPlayerIndex, int32 ElimAmount)
{
	ClientSetHUDScoreBoard(OtherPlayerName, OtherPlayerIndex, ElimAmount);
}

UCharacterScoreBoard* ATFIPlayerController::AddNewScoreBoard(FString NewPlayerName, int32 NewPlayerIndex, int32 NewElimAmount)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->ScoreBoard &&
		ScoreBoardClass;
	if (bHUDValid)
	{
		UCharacterScoreBoard* NewScoreBoard = CreateWidget<UCharacterScoreBoard>(this, ScoreBoardClass);
		bool bScoreBoardValid = NewScoreBoard &&
			NewScoreBoard->PlayerName &&
			NewScoreBoard->PlayerEliminatedAmount;
		if (bScoreBoardValid)
		{
			NewScoreBoard->PlayerName->SetText(FText::FromString(NewPlayerName));
			FString EliminatedAmountText = FString::Printf(TEXT("%d"), NewElimAmount);
			NewScoreBoard->PlayerEliminatedAmount->SetText(FText::FromString(EliminatedAmountText));
			TFICharacterHUD->CharacterOverlay->ScoreBoard->AddChild(NewScoreBoard);
			return NewScoreBoard;
		}
		return nullptr;
	}
	return nullptr;
}

void ATFIPlayerController::SetHUDMissionTarget(float CurrentProgress, float MissionTarget)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->MissionBar &&
		TFICharacterHUD->CharacterOverlay->MissionText;
	if (bHUDValid)
	{
		float Percentage = CurrentProgress / MissionTarget;
		TFICharacterHUD->CharacterOverlay->MissionBar->SetPercent(Percentage);
		FString MissionText = FString::Printf(TEXT("%d / %d"), FMath::FloorToInt(CurrentProgress), FMath::FloorToInt(MissionTarget));
		TFICharacterHUD->CharacterOverlay->MissionText->SetText(FText::FromString(MissionText));
	}
}

void ATFIPlayerController::ClientSetHUDScoreBoard_Implementation(const FString& OtherPlayerName, int32 OtherPlayerIndex, int32 ElimAmount)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->ScoreBoard;
	if (bHUDValid)
	{
		UWidget* ChildWidget = TFICharacterHUD->CharacterOverlay->ScoreBoard->GetChildAt(OtherPlayerIndex);
		UCharacterScoreBoard* ScoreBoard = Cast<UCharacterScoreBoard>(ChildWidget);
		
		if (ScoreBoard == nullptr && ScoreBoardClass)
		{
			ScoreBoard = AddNewScoreBoard(OtherPlayerName, OtherPlayerIndex, ElimAmount);
		}

		bool bScoreBoardValid = ScoreBoard &&
			ScoreBoard->PlayerName &&
			ScoreBoard->PlayerEliminatedAmount;

		if (bScoreBoardValid)
		{
			ScoreBoard->PlayerName->SetText(FText::FromString(OtherPlayerName));
			FString EliminatedAmountText = FString::Printf(TEXT("%d"), ElimAmount);
			ScoreBoard->PlayerEliminatedAmount->SetText(FText::FromString(EliminatedAmountText));
		}
	}
}

float ATFIPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

void ATFIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	if (TFICharacterHUD != nullptr)
	{
		TFICharacterHUD->AddCharacterOverlay();
	}
	SetHUDRespawnNotify(ESlateVisibility::Hidden);

	TFIGameMode = TFIGameMode == nullptr ? Cast<ATFIGameMode>(UGameplayStatics::GetGameMode(GetWorld())) : TFIGameMode;
	if (TFIGameMode != nullptr)
	{
		LevelStartingTime = TFIGameMode->LevelStartingTime;

		if (HasAuthority())
		{
			TFIGameMode->AddToOnlinePlayers(this);
		}
	}
}

void ATFIPlayerController::PollInit()
{
	TFIGameMode = TFIGameMode == nullptr ? Cast<ATFIGameMode>(UGameplayStatics::GetGameMode(GetWorld())) : TFIGameMode;
	if (CharacterOverlay == nullptr)
	{
		if (TFICharacterHUD && TFICharacterHUD->CharacterOverlay)
		{
			CharacterOverlay = TFICharacterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				if (HasAuthority() && bInitializeMissionTarget)
				{
					ATFIGameState* GameState = GetWorld()->GetGameState<ATFIGameState>();
					if (GameState)
					{
						GameState->AddTotalElimAmount(0);
					}
					bInitializeMissionTarget = false;
				}
			}
		}
	}
}

void ATFIPlayerController::SetHUDGameTimer(float Time)
{
	if (HasAuthority())
	{
		TFIGameMode = Cast<ATFIGameMode>(UGameplayStatics::GetGameMode(this));
		if (TFIGameMode != nullptr)
			LevelStartingTime = TFIGameMode->LevelStartingTime;
	}
	Time += LevelStartingTime;
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD != nullptr &&
		TFICharacterHUD->CharacterOverlay != nullptr &&
		TFICharacterHUD->CharacterOverlay->GameTimer != nullptr;
	if (bHUDValid)
	{
		int32 Minutes = Time / 60;
		int32 Seconds = Time - Minutes * 60;

		FString TimerText = FString::Printf(TEXT("%02d 分 %02d 秒"), Minutes, Seconds);
		TFICharacterHUD->CharacterOverlay->GameTimer->SetText(FText::FromString(TimerText));
	}
}

void ATFIPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ATFIPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ATFIPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}
