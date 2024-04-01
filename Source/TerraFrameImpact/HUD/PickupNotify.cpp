// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupNotify.h"
#include "Animation/WidgetAnimation.h"

void UPickupNotify::PlayAppearAnim()
{
	if (AppearAnim)
	{
		PlayAnimationForward(AppearAnim);
		// PlayAnimation(AppearAnim, AppearAnim->GetStartTime(), 1, EUMGSequencePlayMode::Forward);
	}
	StartDisplayTimer();
}

void UPickupNotify::StartDisplayTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		DisplayTimer,
		this,
		&UPickupNotify::DisplayTimerFinished,
		DisplayDuration
	);
}

void UPickupNotify::StartDestroyTimer()
{
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimer,
		this,
		&UPickupNotify::DestroyTimerFinished,
		DestroyDelay
	);
}

void UPickupNotify::DisplayTimerFinished()
{
	if (AppearAnim)
	{
		PlayAnimationReverse(AppearAnim);
		// PlayAnimation(AppearAnim, AppearAnim->GetEndTime(), 1, EUMGSequencePlayMode::Reverse);
	}
	StartDestroyTimer();
}

void UPickupNotify::DestroyTimerFinished()
{
	RemoveFromParent();
	Destruct();
}
