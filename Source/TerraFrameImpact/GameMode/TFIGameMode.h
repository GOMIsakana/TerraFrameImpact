// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TFIGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ATFIGameMode();

	virtual void Tick(float DeltaTime) override;
	virtual void RequestRespawn(ACharacter* ElimmitedCharacter, AController* ElimmedController);
};
