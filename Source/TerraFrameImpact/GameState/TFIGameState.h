// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TFIGameState.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIGameState : public AGameState
{
	GENERATED_BODY()

public:
	void AddTotalElimAmount(int32 AmountToAdd);
	virtual void AddEnemyAmount(int32 AmountToAdd);

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_TotalElimAmount)
	int32 TotalElimAmount = 0;

	UPROPERTY(EditAnywhere, Replicated = true)
	int32 TargetElimAmount = 10;

	UFUNCTION()
	virtual void OnRep_TotalElimAmount();

	UPROPERTY(VisibleAnywhere)
	int32 EnemyAmount = 0;
	UPROPERTY(EditAnywhere)
	int32 EnemyLimit = 10;

public:
	FORCEINLINE int32 GetEnemyAmount() const { return EnemyAmount; }
	FORCEINLINE int32 GetEnemyLimit() const { return EnemyLimit; }
};
