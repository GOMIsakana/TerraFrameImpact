// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIGameState.h"
#include "Net/UnrealNetwork.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"

void ATFIGameState::AddTotalElimAmount(int32 AmountToAdd)
{
	TotalElimAmount = FMath::Max(TotalElimAmount + AmountToAdd, 0);
	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		PlayerController->SetHUDMissionTarget(TotalElimAmount, TargetElimAmount);
	}
}

void ATFIGameState::BeginPlay()
{
	Super::BeginPlay();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ATFIPlayerController* Controller = Cast<ATFIPlayerController>(*It);
		if (Controller != nullptr)
		{
			Controller->SetHUDMissionTarget(TotalElimAmount, TargetElimAmount);
		}
	}
}

void ATFIGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFIGameState, TotalElimAmount);
	DOREPLIFETIME(ATFIGameState, TargetElimAmount);
}

void ATFIGameState::OnRep_TotalElimAmount()
{
	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PlayerController != nullptr)
	{
		PlayerController->SetHUDMissionTarget(TotalElimAmount, TargetElimAmount);
	}
}

void ATFIGameState::AddEnemyAmount(int32 AmountToAdd)
{
	EnemyAmount = FMath::Max(EnemyAmount + AmountToAdd, 0);
}