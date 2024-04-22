// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawn.h"
#include "TerraFrameImpact/AIController/TFIAIController.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TerraFrameImpact/GameState/TFIGameState.h"

// Sets default values
AEnemySpawn::AEnemySpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawn::OnSpawnTimerFinished()
{
	ATFIGameMode* GameMode = GetWorld()->GetAuthGameMode<ATFIGameMode>();
	if (ActorClass == nullptr || GameMode == nullptr || !HasAuthority()) return;
	ATFIGameState* GameState = GameMode->GetGameState<ATFIGameState>();
	if (GameState == nullptr) return;

	FActorSpawnParameters SpawnParmeters;
	SpawnParmeters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	const FTransform SpawnTransform = GetActorTransform();
	for (int i = 0; i < SpawnAmountOneTime && GameState->GetEnemyAmount() < GameState->GetEnemyLimit(); i++)
	{
		ATFIAICharacter* Enemy = GetWorld()->SpawnActor<ATFIAICharacter>(ActorClass, SpawnTransform, SpawnParmeters);
		if (Enemy != nullptr)
		{
			Enemy->SpawnDefaultController();
			Enemy->SetReplicates(true);
			GameState->AddEnemyAmount(1);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("敌人生成错误"))
			return;
		}
	}
	if (TotalSpawnTimes > 0)
	{
		TotalSpawnTimes = FMath::Max(TotalSpawnTimes - SpawnAmountOneTime, 0);
	}
	if (TotalSpawnTimes == 0)
	{
		StopSpawn();
	}
}

// Called every frame
void AEnemySpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

