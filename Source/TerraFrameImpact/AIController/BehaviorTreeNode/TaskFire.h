// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TaskFire.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API UTaskFire : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UTaskFire();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
