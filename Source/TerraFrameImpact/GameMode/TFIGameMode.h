// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TerraFrameImpact/PlayerState/TFIPlayerState.h"
#include "TFIGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ATFIGameMode();

	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void RequestRespawn(ACharacter* ElimmitedCharacter, AController* ElimmedController);
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	void PlayerElimEnemy(ATFIPlayerController* Player, int32 ElimAmount);

	float LevelStartingTime;

	void TryInitScoreBoard(ATFIPlayerController* Controller);

	void PlayerLeftGame(ATFIPlayerState* LeaveingPlayer);

protected:
	void PollInit();
	TArray<ATFIPlayerController*> OnlinePlayers;
	UPROPERTY(Replicated = true)
	TArray<FString> OnlinePlayersName;
	UPROPERTY(Replicated = true)
	TArray<int32> OnlinePlayersIndex;
	TArray<bool> PlayerInitCompleteList;
	int32 LastPlayerIndex = 0;

public:
	FORCEINLINE void AddToOnlinePlayers(ATFIPlayerController* NewPlayer) { OnlinePlayers.Add(NewPlayer); OnlinePlayersName.Add(NewPlayer->GetPlayerState<ATFIPlayerState>()->GetPlayerName()); }
	FORCEINLINE TArray<FString> GetOnlinePlayersName() const { return OnlinePlayersName; }
	FORCEINLINE TArray<int32> GetOnlinePlayersIndex() const { return OnlinePlayersIndex; }
};
