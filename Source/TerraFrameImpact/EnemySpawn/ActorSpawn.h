// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorSpawn.generated.h"

UCLASS()
class TERRAFRAMEIMPACT_API AActorSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorSpawn();

	void StartSpawnTimer();
	void StartSpawn();
	void StopSpawn();
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnSpawnTimerFinished();
	UPROPERTY(EditAnywhere, Category = "生成")
	TSubclassOf<AActor> ActorClass;
	FTimerHandle SpawnTimer;
	UPROPERTY(EditAnywhere, Category = "生成")
	float SpawnDelay = 5.f;
	UPROPERTY(EditAnywhere, Category = "生成")
	bool bSpawnAtFirstTick;
	UPROPERTY(EditAnywhere, Category = "生成")
	int32 SpawnAmountOneTime = 1;
	UPROPERTY(EditAnywhere, Category = "生成")
	int32 TotalSpawnTimes = -1;

};
