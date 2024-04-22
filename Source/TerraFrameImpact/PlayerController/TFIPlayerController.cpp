// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIPlayerController.h"
#include "TerraFrameImpact/HUD/TFICharacterHUD.h"
#include "TerraFrameImpact/HUD/CharacterOverlay.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TerraFrameImpact/GameMode/TFILobbyGameMode.h"
#include "TerraFrameImpact/PlayerState/TFIPlayerState.h"
#include "TerraFrameImpact/GameState/TFIGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TerraFrameImpact/HUD/QuickMenuWidget.h"
#include "TerraFrameImpact/HUD/LobbyOverlay.h"

void ATFIPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDGameTimer(GetServerTime());
	CheckTimeSync(DeltaTime);
	PollInit();
}

void ATFIPlayerController::SwitchHUDMenu()
{
	bCallingMenu ? TearHUDMenu() : CallHUDMenu();
	bCallingMenu ^= true;
}

void ATFIPlayerController::CallHUDMenu()
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->QuickMenu;
	if (bHUDValid)
	{
		TFICharacterHUD->QuickMenu->CallMenu();
	}
}

void ATFIPlayerController::TearHUDMenu()
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->QuickMenu;
	if (bHUDValid)
	{
		TFICharacterHUD->QuickMenu->TearMenu();
	}
}

void ATFIPlayerController::ClientSetHUDPlayerAmount_Implementation(int32 CurrentPlayerAmount, int32 MaxPlayerAmount)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->LobbyOverlay &&
		TFICharacterHUD->LobbyOverlay->PlayerAmountText;
	if (bHUDValid)
	{
		FString AmountString = TEXT("当前房间人数: ") + FString::Printf(TEXT("%d / %d"), CurrentPlayerAmount, MaxPlayerAmount);
		TFICharacterHUD->LobbyOverlay->PlayerAmountText->SetText(FText::FromString(AmountString));
	}
}

void ATFIPlayerController::BeginPlay()
{
	Super::BeginPlay();

	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	if (TFICharacterHUD != nullptr)
	{
		TFICharacterHUD->AddCharacterOverlay();
		TFICharacterHUD->AddStopMenu();
		TFICharacterHUD->AddLobbyOverlay();
	}
	SetHUDRespawnNotify(ESlateVisibility::Hidden);

	TFIGameMode = TFIGameMode == nullptr ? Cast<ATFIGameMode>(UGameplayStatics::GetGameMode(GetWorld())) : TFIGameMode;
	if (TFIGameMode != nullptr)
	{
		LevelStartingTime = TFIGameMode->LevelStartingTime;

		if (HasAuthority())
		{
			TFIGameMode->AddToOnlinePlayers(this);
			ClientSetHUDPlayerAmount(1, 4);
		}
	}
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
	// 检查HUD是否已启用, 并且生命值的ProgressBar和TextBlock组件有效
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->HealthBar &&
		TFICharacterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		// 设置生命值的ProgressBar组件的进度和TextBlock组件所显示的值
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

void ATFIPlayerController::AddNewPickupNotify(FString ItemName, int32 ItemAmount)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->PickupBoard &&
		PickupNotifyClass;
	if (bHUDValid)
	{
		UPickupNotify* NewPickupNotify = CreateWidget<UPickupNotify>(this, PickupNotifyClass);
		if (NewPickupNotify != nullptr)
		{
			if (NewPickupNotify->ItemName && NewPickupNotify->ItemAmount)
			{
				NewPickupNotify->ItemName->SetText(FText::FromString(ItemName));
				FString AmountText = FString::Printf(TEXT(" + %d"), ItemAmount);
				NewPickupNotify->ItemAmount->SetText(FText::FromString(AmountText));
			}
			TFICharacterHUD->CharacterOverlay->PickupBoard->AddChild(NewPickupNotify);
			NewPickupNotify->PlayAppearAnim();
		}
	}
	ClientAddNewPickupNotify(ItemName, ItemAmount);
}

void ATFIPlayerController::ClientAddNewPickupNotify_Implementation(const FString& ItemName, int32 ItemAmount)
{
	if (HasAuthority()) return;
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->PickupBoard &&
		PickupNotifyClass;
	if (bHUDValid)
	{
		UPickupNotify* NewPickupNotify = CreateWidget<UPickupNotify>(this, PickupNotifyClass);
		if (NewPickupNotify != nullptr)
		{
			if (NewPickupNotify->ItemName && NewPickupNotify->ItemAmount)
			{
				NewPickupNotify->ItemName->SetText(FText::FromString(ItemName));
				FString AmountText = FString::Printf(TEXT(" + %d"), ItemAmount);
				NewPickupNotify->ItemAmount->SetText(FText::FromString(AmountText));
			}
			TFICharacterHUD->CharacterOverlay->PickupBoard->AddChild(NewPickupNotify);
			NewPickupNotify->PlayAppearAnim();
		}
	}
}

void ATFIPlayerController::SetHUDMissionTarget(const FString& MissionName, float CurrentProgress, float MissionTarget)
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
		if (Percentage < 1)
		{
			FString MissionText = MissionName + FString::Printf(TEXT("%d / %d"), FMath::FloorToInt(CurrentProgress), FMath::FloorToInt(MissionTarget));
			TFICharacterHUD->CharacterOverlay->MissionText->SetText(FText::FromString(MissionText));
		}
		else
		{
			FString MissionText = TEXT("任务完成 - 按下[ ESC ]呼出菜单即可离开游戏");
			TFICharacterHUD->CharacterOverlay->MissionText->SetText(FText::FromString(MissionText));
		}
	}
}

void ATFIPlayerController::SetHUDRespawnTimes(int32 RemainTimes, int32 MaxTimes)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->RespawnText;
	if (bHUDValid)
	{
		FString RespawnText = FString::Printf(TEXT("全队剩余重生次数: %d / %d"), RemainTimes, MaxTimes);
		TFICharacterHUD->CharacterOverlay->RespawnText->SetText(FText::FromString(RespawnText));
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
			FString PlayerNameText = OtherPlayerName;
			if (OtherPlayerName.Len() > 10)
			{
				PlayerNameText = OtherPlayerName.Left(10) + TEXT("...");
			}
			PlayerNameText = PlayerNameText + TEXT(" - ");
			
			ScoreBoard->PlayerName->SetText(FText::FromString(PlayerNameText));
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

void ATFIPlayerController::SetHUDMinimap(FVector CharacterLocation)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->Minimap &&
		TFICharacterHUD->CharacterOverlay->Minimap->MapImage;
	if (bHUDValid)
	{
		FVector2D SizeBoxOffset = FVector2D(TFICharacterHUD->CharacterOverlay->Minimap->MapClippingSizeBox->GetWidthOverride(), TFICharacterHUD->CharacterOverlay->Minimap->MapClippingSizeBox->GetHeightOverride());
		FVector2D ImageDesiredSize = TFICharacterHUD->CharacterOverlay->Minimap->MapImage->GetBrush().GetImageSize();
		FVector2D MapWorldSize = FVector2D(50400.f, 50400.f);
		FVector2D CharacterLocation2D = FVector2D(CharacterLocation.X, CharacterLocation.Y);
		// 第一个除是获取玩家当前位置在世界中的相对比例(0 ~ 100)%；第二个乘是将这个比例放到图片对应的中心位置；
		// 又因为图片对齐的是左上角，想看到右下角的图片内容需要将图片移动到负坐标轴；最后减去这部分是因为地图有正有负，减去图片size/2可以保证对齐到右上角
		FVector2D MapRenderTranslation = CharacterLocation2D / MapWorldSize * ImageDesiredSize * -1.f - 2500.f + SizeBoxOffset / 2.f;
		TFICharacterHUD->CharacterOverlay->Minimap->MapImage->SetRenderTranslation(MapRenderTranslation);
	}
}

void ATFIPlayerController::SetHUDTitleMessage(const FString& Message, const FSlateColor& TextColor, float RemoveDelay)
{
	ClientSetHUDTitleMessage(Message, TextColor, RemoveDelay);
}

void ATFIPlayerController::ClientSetHUDTitleMessage_Implementation(const FString& Message, const FSlateColor& TextColor, float RemoveDelay)
{
	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->TitleMessage;
	if (bHUDValid)
	{
		TFICharacterHUD->CharacterOverlay->TitleMessage->SetVisibility(ESlateVisibility::Visible);
		TFICharacterHUD->CharacterOverlay->TitleMessage->SetText(FText::FromString(Message));
		TFICharacterHUD->CharacterOverlay->TitleMessage->SetColorAndOpacity(TextColor);
		UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(TitleRemoveTimer);
			World->GetTimerManager().SetTimer(
				TitleRemoveTimer,
				this,
				&ATFIPlayerController::OnTitleRemoveTimerFinished,
				RemoveDelay
			);
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
				if (HasAuthority() && bInitializeRespawnTimes)
				{
					ATFIGameState* GameState = GetWorld()->GetGameState<ATFIGameState>();
					if (GameState)
					{
						GameState->AddRespawnTimes(0);
					}
					bInitializeRespawnTimes = false;
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

void ATFIPlayerController::OnTitleRemoveTimerFinished()
{

	TFICharacterHUD = TFICharacterHUD == nullptr ? Cast<ATFICharacterHUD>(GetHUD()) : TFICharacterHUD;
	bool bHUDValid = TFICharacterHUD &&
		TFICharacterHUD->CharacterOverlay &&
		TFICharacterHUD->CharacterOverlay->TitleMessage;
	if (bHUDValid)
	{
		TFICharacterHUD->CharacterOverlay->TitleMessage->SetVisibility(ESlateVisibility::Hidden);
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
