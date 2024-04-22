// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "TFIAIController.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	void StartExecuteAI();
	void StopExecuteAI();

protected:
	virtual void Tick(float DeltaTime) override;
	void ExecuteAI();
	virtual void Destroyed() override;

private:
	class ATFICharacter* CurrentTarget;
	FTimerHandle ExecuteAITimer;
	UPROPERTY(EditAnywhere, Category = "AI行为参数")
	class UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "AI行为参数")
	float ExecuteAIDelay = .5f;

	UPROPERTY(EditAnywhere, Category = "AI行为参数")
	float WanderRange = 500.f;

	UPROPERTY(EditAnywhere, Category = "AI行为参数")
	float AttackDistance = 2000.f;

	UPROPERTY(EditAnywhere, Category = "AI行为参数")
	float DashDistance = 2000.f;

	TArray<AActor*> PlayerCharacterArray;
	FVector CurrentTargetLocation;
	FVector LastTargetLocation;

	class ATFIAICharacter* Character;

	bool bBehaviorTreeExecuting = false;

public:
	FORCEINLINE FVector GetCurrentTargetLocationn() const { return CurrentTargetLocation; }
	FORCEINLINE ATFICharacter* GetCurrentTarget() const { return CurrentTarget; }
	FORCEINLINE bool IsBehaviorTreeExecuting() const { return bBehaviorTreeExecuting; }
	FVector GetTargetHeadLocation();
	bool bCurrentTargetValid();

};
