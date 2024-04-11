// Fill out your copyright notice in the Description page of Project Settings.


#include "TFILobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"

void ATFILobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

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
}
