// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFICombatComponent.h"
#include "TFIAICombatComponent.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API UTFIAICombatComponent : public UTFICombatComponent
{
	GENERATED_BODY()
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AIFire();

	UFUNCTION(Server, Reliable)
	void ServerAIFire();
};
