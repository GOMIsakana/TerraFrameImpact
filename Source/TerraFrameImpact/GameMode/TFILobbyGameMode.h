// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TerraFrameImpact/GameMode/TFIGameMode.h"
#include "TFILobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFILobbyGameMode : public ATFIGameMode
{
	GENERATED_BODY()
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	virtual void PollInit(APlayerController* TargetPlayer) override;
};
