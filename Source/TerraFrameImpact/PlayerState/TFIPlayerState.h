// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "TFIPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	void AddPlayerElimAmount(int32 AmountToAdd);

	FORCEINLINE int32 GetPlayerElimAmount() const { return PlayerElimAmount; }

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY()
	ATFICharacter* Character;
	UPROPERTY()
	ATFIPlayerController* Controller;

	UPROPERTY(VisibleAnywhere, Replicated=true)
	int32 PlayerElimAmount = 0;
};
