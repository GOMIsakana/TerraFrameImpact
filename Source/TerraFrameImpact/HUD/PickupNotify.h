// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PickupNotify.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API UPickupNotify : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ItemAmount;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* AppearAnim;

	FTimerHandle DisplayTimer;
	FTimerHandle DestroyTimer;

	void PlayAppearAnim();

protected:
	void StartDisplayTimer();
	void StartDestroyTimer();

	UFUNCTION()
	void DisplayTimerFinished();
	UFUNCTION()
	void DestroyTimerFinished();

	UPROPERTY(EditAnywhere)
	float DisplayDuration = 3.f;
	UPROPERTY(EditAnywhere)
	float DestroyDelay = .5f;
};
