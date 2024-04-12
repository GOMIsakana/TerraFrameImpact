// Fill out your copyright notice in the Description page of Project Settings.


#include "TFISurvivalGameState.h"
#include "TerraFrameImpact/GameMode/TFISurvivalGameMode.h"
#include "Net/UnrealNetwork.h"

void ATFISurvivalGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATFISurvivalGameMode* SurvivalGameMode = Cast<ATFISurvivalGameMode>(GetWorld()->GetAuthGameMode());
	float CurrentWorldTime = GetWorld()->GetTimeSeconds();
	if (!bGameFinished && SurvivalGameMode)
	{
		SurvivalTime = CurrentWorldTime - SurvivalGameMode->LevelStartingTime;
		ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
		if (PlayerController != nullptr)
		{
			PlayerController->SetHUDMissionTarget(TEXT("生存指定时间(秒): "), SurvivalTime, TargetSurvivalTime);
		}
	}
		
	if (CheckMissionFinish() && !bGameFinished)
	{
		bGameFinished = true;

		ATFISurvivalGameMode* TFISurvivalGameMode = Cast<ATFISurvivalGameMode>(GetWorld()->GetAuthGameMode());
		if (TFISurvivalGameMode)
		{
			TFISurvivalGameMode->EndGame();
		}
	}
}

void ATFISurvivalGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFISurvivalGameState, SurvivalTime);
}

bool ATFISurvivalGameState::CheckMissionFinish()
{
	Super::CheckMissionFinish();
	return SurvivalTime >= TargetSurvivalTime;
}

void ATFISurvivalGameState::OnRep_SurvivalTime()
{
	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		PlayerController->SetHUDMissionTarget(TEXT("生存指定时间(秒): "), SurvivalTime, TargetSurvivalTime);
	}
}
