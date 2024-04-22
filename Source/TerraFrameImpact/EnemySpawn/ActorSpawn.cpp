// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorSpawn.h"
#include "TerraFrameImpact/AIController/TFIAIController.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TerraFrameImpact/GameState/TFIGameState.h"

// Sets default values
AActorSpawn::AActorSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AActorSpawn::StartSpawnTimer()
{
	if (!HasAuthority()) return;
	GetWorld()->GetTimerManager().SetTimer(
		SpawnTimer,
		this,
		&AActorSpawn::OnSpawnTimerFinished,
		SpawnDelay,
		true,
		SpawnDelay
	);
}

void AActorSpawn::StartSpawn()
{
	if (bSpawnAtFirstTick)
	{
		OnSpawnTimerFinished();
	}
	StartSpawnTimer();
}

void AActorSpawn::StopSpawn()
{
	GetWorld()->GetTimerManager().ClearTimer(SpawnTimer);
}

// Called when the game starts or when spawned
void AActorSpawn::BeginPlay()
{
	Super::BeginPlay();

	StartSpawn();
}

void AActorSpawn::OnSpawnTimerFinished()
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
		AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorClass, SpawnTransform, SpawnParmeters);
		if (Actor != nullptr)
		{
			// Actor->SpawnDefaultController();
			Actor->SetReplicates(true);
			// GameState->AddEnemyAmount(1);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("生成错误"));
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
void AActorSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

