// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFIGameMode.h"
#include "TFISurvivalGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFISurvivalGameMode : public ATFIGameMode
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
};
