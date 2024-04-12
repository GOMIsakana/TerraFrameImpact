// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIGameMode.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TerraFrameImpact/PlayerState/TFIPlayerState.h"
#include "TerraFrameImpact/GameState/TFIGameState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetSystemLibrary.h"

ATFIGameMode::ATFIGameMode()
{

}

void ATFIGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 未解决：什么时候停止init，一直init不是办法
	for (FConstPlayerControllerIterator OutIt = GetWorld()->GetPlayerControllerIterator(); OutIt; OutIt++)
	{
		PollInit(Cast<APlayerController>(*OutIt));
	}
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
	ATFIGameState* TFIGameState = Cast<ATFIGameState>(GameState);
	if (ElimmitedCharacter)
	{
		ElimmitedCharacter->Reset();
		ElimmitedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		RestartPlayerAtTransform(ElimmedController, ElimmitedCharacter->GetActorTransform());
	}
	if (TFIGameState)
	{
		TFIGameState->AddRespawnTimes(-1);
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
	}
}

void ATFIGameMode::PlayerLeftGame(ATFIPlayerState* LeaveingPlayer)
{
	if (LeaveingPlayer == nullptr) return;
	ATFICharacter* CharacterLeaving = Cast<ATFICharacter>(LeaveingPlayer->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Destroy();
	}
}

void ATFIGameMode::EndGame()
{
	GetWorld()->GetTimerManager().SetTimer(
		EndGameTimer,
		this,
		&ATFIGameMode::OnEndGameTimerFinish,
		1.f
	);
}

void ATFIGameMode::OnEndGameTimerFinish()
{
	EndCountdown -= 1.f;
	if (EndCountdown > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			EndGameTimer,
			this,
			&ATFIGameMode::OnEndGameTimerFinish,
			1.f
		);
		for (FConstPlayerControllerIterator OutIt = GetWorld()->GetPlayerControllerIterator(); OutIt; OutIt++)
		{
			ATFIPlayerController* ThisPlayerController = Cast<ATFIPlayerController>(*OutIt);

			if (ThisPlayerController != nullptr)
			{
				FSlateColor Color = FColor::Orange;
				FString EndGameText = FString::Printf(TEXT("游戏即将结束: %.0f"), EndCountdown);
				ThisPlayerController->SetHUDTitleMessage(EndGameText, Color, 2.f);
			}
		}
	}
	else
	{
		for (FConstPlayerControllerIterator OutIt = GetWorld()->GetPlayerControllerIterator(); OutIt; OutIt++)
		{
			ATFIPlayerController* ThisPlayerController = Cast<ATFIPlayerController>(*OutIt);

			if (ThisPlayerController != nullptr)
			{
				FSlateColor Color = FColor::Orange;
				ThisPlayerController->SetHUDTitleMessage(TEXT("游戏已结束\n按下[ ESC ]打开菜单重新开始或离开游戏"), Color, 10.f);
			}
		}
	}
}

void ATFIGameMode::PollInit(APlayerController* TargetPlayer)
{
	ATFIPlayerController* ThisPlayerController = Cast<ATFIPlayerController>(TargetPlayer);

	if (ThisPlayerController != nullptr)
	{
		TryInitScoreBoard(ThisPlayerController);
	}
}
