// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorSpawn.h"
#include "TerraFrameImpact/Character/TFIAICharacter.h"
#include "EnemySpawn.generated.h"

UCLASS()
class TERRAFRAMEIMPACT_API AEnemySpawn : public AActorSpawn
{
	GENERATED_BODY()
	
public:	
	AEnemySpawn();

protected:

	virtual void OnSpawnTimerFinished() override;

private:
	FTimerHandle SpawnTimer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
