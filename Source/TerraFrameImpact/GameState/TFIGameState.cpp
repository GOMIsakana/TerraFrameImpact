// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIGameState.h"
#include "Net/UnrealNetwork.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"

ATFIGameState::ATFIGameState()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATFIGameState::AddTotalElimAmount(int32 AmountToAdd)
{
	TotalElimAmount = FMath::Max(TotalElimAmount + AmountToAdd, 0);
}

void ATFIGameState::BeginPlay()
{
	Super::BeginPlay();

	RemainRespawnTimes = MaxRespawnTimes;

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ATFIPlayerController* Controller = Cast<ATFIPlayerController>(*It);
		if (Controller != nullptr)
		{
			Controller->SetHUDRespawnTimes(RemainRespawnTimes, MaxRespawnTimes);
		}
	}
}

void ATFIGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFIGameState, TotalElimAmount);
	DOREPLIFETIME(ATFIGameState, RemainRespawnTimes);
}

bool ATFIGameState::CheckMissionFinish()
{
	return false;
}

void ATFIGameState::OnRep_TotalElimAmount()
{
}

void ATFIGameState::AddEnemyAmount(int32 AmountToAdd)
{
	EnemyAmount = FMath::Max(EnemyAmount + AmountToAdd, 0);

}

void ATFIGameState::AddRespawnTimes(int32 AmountToAdd)
{
	RemainRespawnTimes += AmountToAdd;
	// 次数小于0的时候说明是在复活次数=0的时候请求减少的，所以负了就是直接判输
	if (RemainRespawnTimes < 0)
	{
		ATFIGameMode* TFIGameMode = Cast<ATFIGameMode>(GetWorld()->GetAuthGameMode());
		if (TFIGameMode)
		{
			TFIGameMode->EndGame();
		}
		return;
	}

	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		PlayerController->SetHUDRespawnTimes(RemainRespawnTimes, MaxRespawnTimes);
	}
}

void ATFIGameState::OnRep_RemainRespawnTimes()
{
	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		PlayerController->SetHUDRespawnTimes(RemainRespawnTimes, MaxRespawnTimes);
	}
}