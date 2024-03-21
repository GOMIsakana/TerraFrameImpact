// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"

void ATFIPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFIPlayerState, PlayerElimAmount);
}

void ATFIPlayerState::AddPlayerElimAmount(int32 AmountToAdd)
{
	PlayerElimAmount = FMath::Max(PlayerElimAmount + AmountToAdd, 0);
	Character = Character == nullptr ? Cast<ATFICharacter>(GetPawn()) : Character;
	if (Character != nullptr)
	{
		Controller = Controller == nullptr ? Cast<ATFIPlayerController>(Character->GetController()) : Controller;
		if (Controller != nullptr)
		{
			Controller->SetHUDScoreBoard(Controller->GetName(), Controller->GetPlayerIndex(), PlayerElimAmount);
		}
	}
}
