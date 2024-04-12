// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TerraFrameImpact/HUD/CharacterScoreBoard.h"
#include "TerraFrameImpact/HUD/PickupNotify.h"
#include "TFIPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);

	void SetHUDHealth(float CurHealth, float MaxHealth);
	void SetHUDShield(float CurShield, float MaxShield);

	void SetHUDRespawnNotify(ESlateVisibility State);

	void SetHUDScoreBoard(const FString& OtherPlayerName, int32 OtherPlayerIndex, int32 ElimAmount);
	UFUNCTION(Client, Reliable)
	void ClientSetHUDScoreBoard(const FString& OtherPlayerName, int32 OtherPlayerIndex, int32 ElimAmount);
	UCharacterScoreBoard* AddNewScoreBoard(FString NewPlayerName, int32 NewPlayerIndex, int32 NewElimAmount);

	void AddNewPickupNotify(FString ItemName, int32 ItemAmount);
	UFUNCTION(Client, Reliable)
	void ClientAddNewPickupNotify(const FString& ItemName, int32 ItemAmount);

	void SetHUDMissionTarget(const FString& MissionName, float CurrentProgress, float MissionTarget);

	void SetHUDRespawnTimes(int32 RemainTimes, int32 MaxTimes);

	float GetServerTime();

	void SetHUDMinimap(FVector CharacterLocation);

	void SetHUDTitleMessage(const FString& Message, const FSlateColor& TextColor, float RemoveDelay);

	UFUNCTION(Client, Reliable)
	void ClientSetHUDTitleMessage(const FString& Message, const FSlateColor& TextColor, float RemoveDelay);

	void SwitchHUDMenu();
	void CallHUDMenu();
	void TearHUDMenu();

	UFUNCTION(Client, Reliable)
	void ClientSetHUDPlayerAmount(int32 CurrentPlayerAmount, int32 MaxPlayerAmount);

protected:
	virtual void BeginPlay() override;
	void PollInit();
	void SetHUDGameTimer(float Time);

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

private:
	UPROPERTY()
	class ATFICharacterHUD* TFICharacterHUD;
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;
	float HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCharacterScoreBoard> ScoreBoardClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPickupNotify> PickupNotifyClass;

	float SingleTripTime;
	float ClientServerDelta;
	float TimeSyncRunningTime = 0.f;
	float TimeSyncFrequency = 5.f;
	float LevelStartingTime;
	void CheckTimeSync(float DeltaTime);

	TArray<FString> PollInitPlayerNameList;
	TArray<int32> PollInitPlayerIndexList;

	class ATFIGameMode* TFIGameMode;

	bool bInitializeMissionTarget = true;
	bool bInitializeRespawnTimes = true;

	int32 PlayerIndex = 0;

	FTimerHandle TitleRemoveTimer;
	void OnTitleRemoveTimerFinished();

	bool bCallingMenu = false;

public:
	FORCEINLINE int32 GetPlayerIndex() const { return PlayerIndex; }
	FORCEINLINE void SetPlayerIndex(int32 Index) { PlayerIndex = Index; }
	FORCEINLINE void SetPollInitPlayerNameList(const TArray<FString>& PlayerNameList) { PollInitPlayerNameList = PlayerNameList; }
	FORCEINLINE void SetPollInitPlayerIndexList(const TArray<int32>& PlayerIndexList) { PollInitPlayerIndexList = PlayerIndexList; }
	FORCEINLINE bool GetCallingMenu() const { return bCallingMenu; }
};
