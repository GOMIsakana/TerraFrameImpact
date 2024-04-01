// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractActor.h"
#include "Engine/TimelineTemplate.h"
#include "Components/TimelineComponent.h"
#include "InteractDoor.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API AInteractDoor : public AInteractActor
{
	GENERATED_BODY()

public:
	AInteractDoor();

	virtual void Interact() override;

protected:
	UPROPERTY(EditAnywhere, Category = "InteractDoor")
	FTransform TargetTransform;
	UPROPERTY(EditAnywhere, Category = "InteractDoor")
	float MoveDelay;
	UPROPERTY(EditAnywhere, Category = "InteractDoor")
	UCurveVector* MoveLocationCurve;
	UPROPERTY(EditAnywhere, Category = "InteractDoor")
	UCurveVector* MoveRotationCurve;
	UPROPERTY(EditAnywhere, Category = "InteractDoor")
	UCurveVector* MoveScaleCurve;
	FOnTimelineVector MoveTimeLocationTrack;
	FOnTimelineVector MoveTimeRotationTrack;
	FOnTimelineVector MoveTimeScaleTrack;
	UTimelineComponent* MoveTimelineComponent;
	UFUNCTION()
	void UpdateMoveLocation(FVector Location);
	UFUNCTION()
	void UpdateMoveRotation(FVector Rotation);
	UFUNCTION()
	void UpdateMoveScale(FVector Scale);
};
