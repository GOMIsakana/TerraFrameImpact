// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickMenuWidget.h"

void UQuickMenuWidget::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Hidden);
}

void UQuickMenuWidget::CallMenu()
{
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);
	APlayerController* OwnerController = GetOwningPlayer();
	if (OwnerController)
	{
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		OwnerController->SetInputMode(InputModeData);
		OwnerController->SetShowMouseCursor(true);
	}
}

void UQuickMenuWidget::TearMenu()
{
	SetVisibility(ESlateVisibility::Hidden);
	SetIsFocusable(false);
	APlayerController* OwnerController = GetOwningPlayer();
	if (OwnerController)
	{
		FInputModeGameOnly InputModeData;
		OwnerController->SetInputMode(InputModeData);
		OwnerController->SetShowMouseCursor(false);
	}
}