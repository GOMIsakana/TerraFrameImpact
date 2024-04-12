// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFIGameState.h"
#include "TFISurvivalGameState.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFISurvivalGameState : public ATFIGameState
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CheckMissionFinish() override;

	UPROPERTY(EditAnywhere)
	float TargetSurvivalTime = 60.f;

	UPROPERTY(ReplicatedUsing = OnRep_SurvivalTime)
	float SurvivalTime = 0.f;

	bool bGameFinished = false;
	UFUNCTION()
	void OnRep_SurvivalTime();
};
