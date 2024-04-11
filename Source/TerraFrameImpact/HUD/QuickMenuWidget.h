// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "QuickMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API UQuickMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup();

	UFUNCTION(BlueprintCallable)
	void CallMenu();

	UFUNCTION(BlueprintCallable)
	void TearMenu();
	/*
	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* StartGameButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* StartGameButtonText;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UButton* LeftGameButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LeftGameButtonText;
	*/
};
