// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIAnnihilateMissionGameState.h"
#include "Net/UnrealNetwork.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TerraFrameImpact/GameMode/TFIAnnihilateGameMode.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"

void ATFIAnnihilateMissionGameState::AddTotalElimAmount(int32 AmountToAdd)
{
	Super::AddTotalElimAmount(AmountToAdd);

	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		PlayerController->SetHUDMissionTarget(TEXT("歼灭敌人: "), TotalElimAmount, TargetElimAmount);
		if (CheckMissionFinish())
		{
			FSlateColor TitleColor = FSlateColor(FColor::Cyan);
			PlayerController->SetHUDTitleMessage(TEXT("- 任务完成 -"), TitleColor, 5.f);
			GetWorld()->GetTimerManager().SetTimer(
				EndGameTimer,
				this,
				&ThisClass::OnEndGameTimerFinished,
				5.f
			);
		}
	}
}

void ATFIAnnihilateMissionGameState::BeginPlay()
{
	Super::BeginPlay();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ATFIPlayerController* Controller = Cast<ATFIPlayerController>(*It);
		if (Controller != nullptr)
		{
			Controller->SetHUDMissionTarget(TEXT("歼灭敌人: "), TotalElimAmount, TargetElimAmount);
		}
	}
}

void ATFIAnnihilateMissionGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFIAnnihilateMissionGameState, TargetElimAmount);
}

bool ATFIAnnihilateMissionGameState::CheckMissionFinish()
{
	return TotalElimAmount >= TargetElimAmount;
}

void ATFIAnnihilateMissionGameState::OnRep_TotalElimAmount()
{
	Super::OnRep_TotalElimAmount();

	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		PlayerController->SetHUDMissionTarget(TEXT("歼灭敌人: "), TotalElimAmount, TargetElimAmount);
		if (CheckMissionFinish())
		{
			FSlateColor TitleColor = FSlateColor(FColor::Cyan);
			PlayerController->SetHUDTitleMessage(TEXT("- 任务完成 -"), TitleColor, 5.f);
		}
	}
}

void ATFIAnnihilateMissionGameState::OnEndGameTimerFinished()
{
	ATFIAnnihilateGameMode* AnnihilateGameMode = Cast<ATFIAnnihilateGameMode>(GetWorld()->GetAuthGameMode());
	if (AnnihilateGameMode)
	{
		AnnihilateGameMode->EndGame();
	}
}
