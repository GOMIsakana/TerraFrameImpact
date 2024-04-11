// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFIGameState.h"
#include "TFIAnnihilateMissionGameState.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIAnnihilateMissionGameState : public ATFIGameState
{
	GENERATED_BODY()

public:
	virtual void AddTotalElimAmount(int32 AmountToAdd) override;

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool CheckMissionFinish() override;

	UPROPERTY(EditAnywhere, Replicated = true)
	int32 TargetElimAmount = 10;

	virtual void OnRep_TotalElimAmount() override;
};
