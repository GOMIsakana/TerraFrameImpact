// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/WrapBox.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"
#include "MinimapWidget.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	USizeBox* MapClippingSizeBox;
	UPROPERTY(meta = (BindWidget))
	UImage* PlayerLocationImage;
	UPROPERTY(meta = (BindWidget))
	UImage* MapImage;
	UPROPERTY(meta = (BindWidget))
	UOverlay* ImageOverlay;
	UPROPERTY(meta = (BindWidget))
	UWrapBox* MapWrapBox;
};
