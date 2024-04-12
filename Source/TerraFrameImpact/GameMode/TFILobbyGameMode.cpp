// Fill out your copyright notice in the Description page of Project Settings.


#include "TFILobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"

void ATFILobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	/*
	ATFIPlayerController* NewPlayerController = Cast<ATFIPlayerController>(NewPlayer);
	if (NewPlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("NewPlayerController"))
		NewPlayerController->ClientSetHUDPlayerAmount(NumberOfPlayers, 4);
	}
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);

		if (NumberOfPlayers == 4)
		{
			UWorld* World = GetWorld();
			if (World)
			{
				Subsystem->StartSession();
				bUseSeamlessTravel = true;

				World->ServerTravel(FString("/Game/Map/AnnihilateMissionLevel?listen"));
			}
		}
	}
	*/
}

void ATFILobbyGameMode::PollInit(APlayerController* TargetPlayer)
{
	Super::PollInit(TargetPlayer);

	ATFIPlayerController* PlayerController = Cast<ATFIPlayerController>(TargetPlayer);
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (PlayerController)
	{
		PlayerController->ClientSetHUDPlayerAmount(NumberOfPlayers, 4);
	}
}