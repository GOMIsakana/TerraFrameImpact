// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerraFrameImpact/Character/TFIAICharacter.h"
#include "EnemySpawn.generated.h"

UCLASS()
class TERRAFRAMEIMPACT_API AEnemySpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawn();

	void StartSpawnTimer();
	void StartSpawn();
	void StopSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnSpawnTimerFinished();

private:
	FTimerHandle SpawnTimer;
	UPROPERTY(EditAnywhere, Category = "敌人生成")
	float SpawnDelay;
	UPROPERTY(EditAnywhere, Category = "敌人生成")
	bool bSpawnAtFirstTick;
	UPROPERTY(EditAnywhere, Category = "敌人生成")
	int32 SpawnAmountOneTime = 1;
	UPROPERTY(EditAnywhere, Category = "敌人生成")
	TSubclassOf<ATFIAICharacter> EnemyClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
