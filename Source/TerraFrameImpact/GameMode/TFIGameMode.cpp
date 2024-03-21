// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIGameMode.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TerraFrameImpact/PlayerState/TFIPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

ATFIGameMode::ATFIGameMode()
{

}

void ATFIGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PollInit();
}

void ATFIGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();

}

void ATFIGameMode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATFIGameMode, OnlinePlayersName);
	DOREPLIFETIME(ATFIGameMode, OnlinePlayersIndex);
}

void ATFIGameMode::RequestRespawn(ACharacter* ElimmitedCharacter, AController* ElimmedController)
{
	if (ElimmitedCharacter)
	{
		ElimmitedCharacter->Reset();
		ElimmitedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		RestartPlayerAtTransform(ElimmedController, ElimmitedCharacter->GetActorTransform());
	}
}

void ATFIGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ATFIPlayerController* NewPlayerController = Cast<ATFIPlayerController>(NewPlayer);
	if (NewPlayerController != nullptr)
	{
		NewPlayerController->SetPlayerIndex(LastPlayerIndex++);
		// OnlinePlayersIndex.Add(LastPlayerIndex);
		// LastPlayerIndex++;
		OnlinePlayersName.Add(NewPlayerController->GetName());
		// OnlinePlayersElimAmount.Add(0);
		ATFIPlayerState* NewPlayerState = NewPlayerController->GetPlayerState<ATFIPlayerState>();
		FString PlayerName = "void";
		if (NewPlayerState != nullptr)
		{
			PlayerName = NewPlayerState->GetPlayerName();
		}
		NewPlayerController->SetHUDScoreBoard(PlayerName, NewPlayerController->GetPlayerIndex(), 0);
		OnlinePlayers.AddUnique(NewPlayerController);
		PlayerInitCompleteList.Reset(LastPlayerIndex);
		// NewPlayerController->SetPollInitPlayerNameList(OnlinePlayersName);
		// NewPlayerController->SetPollInitPlayerIndexList(OnlinePlayersIndex);

		// 给其他人增加这个玩家的数据, 给这个玩家增加其他人的数据

		// 目前问题：这个函数实际上没有执行
	}
}

void ATFIGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

}

void ATFIGameMode::PlayerElimEnemy(ATFIPlayerController* Player, int32 ElimAmount)
{
	ATFIPlayerState* PlayerState = Player == nullptr ? nullptr : Player->GetPlayerState<ATFIPlayerState>();
	if (PlayerState != nullptr)
	{
		PlayerState->AddPlayerElimAmount(1);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			ATFIPlayerController* Controller = Cast<ATFIPlayerController>(*It);
			if (Controller != nullptr)
			{
				Controller->SetHUDScoreBoard(PlayerState->GetPlayerName(), Player->GetPlayerIndex(), PlayerState->GetPlayerElimAmount());
			}
		}
	}
}

void ATFIGameMode::TryInitScoreBoard(ATFIPlayerController* Controller)
{
	if (Controller != nullptr)
	{
		int32 InitSuccessCnt = 0, TotalCnt = 0;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			ATFIPlayerController* OtherPlayerController = Cast<ATFIPlayerController>(*It);
			// UE_LOG(LogTemp, Warning, TEXT("%s: OtherPlayerIndex = %d"), OtherPlayerController->HasAuthority() ? TEXT("Server") : TEXT("Client"), OtherPlayerController->GetPlayerIndex())
			if (OtherPlayerController == nullptr) continue;
			ATFIPlayerState* OtherPlayerState = OtherPlayerController->GetPlayerState<ATFIPlayerState>();
			if (OtherPlayerState != nullptr)
			{
				Controller->SetHUDScoreBoard(OtherPlayerState->GetPlayerName(),
					OtherPlayerController->GetPlayerIndex(),
					OtherPlayerState->GetPlayerElimAmount());
			}
		}
		// Controller->SetPlayerInitializeScoreBoard(!(Controller->GetScoreBoardLength() == LastPlayerIndex));
	}
}

void ATFIGameMode::PollInit()
{
	// 未解决：什么时候停止init，一直init不是办法
	for (FConstPlayerControllerIterator OutIt = GetWorld()->GetPlayerControllerIterator(); OutIt; OutIt++)
	{
		ATFIPlayerController* ThisPlayerController = Cast<ATFIPlayerController>(*OutIt);

		if (ThisPlayerController != nullptr)
		{
			TryInitScoreBoard(ThisPlayerController);
		}
		/*
		if (ThisPlayerController != nullptr)
		{
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
			{
				ATFIPlayerController* OtherPlayerController = Cast<ATFIPlayerController>(*It);
				// UE_LOG(LogTemp, Warning, TEXT("%s: OtherPlayerIndex = %d"), OtherPlayerController->HasAuthority() ? TEXT("Server") : TEXT("Client"), OtherPlayerController->GetPlayerIndex())
				if (OtherPlayerController == nullptr) continue;
				ATFIPlayerState* OtherPlayerState = OtherPlayerController->GetPlayerState<ATFIPlayerState>();
				if (OtherPlayerState != nullptr)
				{
					ThisPlayerController->SetHUDScoreBoard(OtherPlayerState->GetPlayerName(), 
						OtherPlayerController->GetPlayerIndex(), 
						OtherPlayerState->GetPlayerElimAmount());
				}
			}
		}
		*/
	}
}
