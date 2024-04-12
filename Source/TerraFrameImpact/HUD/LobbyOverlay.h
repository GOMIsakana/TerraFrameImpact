// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "LobbyOverlay.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ULobbyOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerAmountText;
};
