// Fill out your copyright notice in the Description page of Project Settings.


#include "TFICharacterHUD.h"
#include "GameFramework/PlayerController.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "TerraFrameImpact/HUD/CharacterOverlay.h"
#include "TerraFrameImpact/HUD/QuickMenuWidget.h"

void ATFICharacterHUD::DrawHUD()
{
	Super::DrawHUD();

	FVector2D ViewportSize;

	// UE_LOG(LogTemp, Warning, TEXT("Drawing HUD"))

	if (GEngine)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

		float SpreadScaled = CrosshairSpreadMax * CrosshairPackage.CrosshairSpread;

		if (CrosshairPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(CrosshairPackage.CrosshairsLeft, ViewportCenter, Spread, CrosshairPackage.CrosshairsColor);
		}
		if (CrosshairPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(CrosshairPackage.CrosshairsRight, ViewportCenter, Spread, CrosshairPackage.CrosshairsColor);
		}
		if (CrosshairPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(CrosshairPackage.CrosshairsTop, ViewportCenter, Spread, CrosshairPackage.CrosshairsColor);
		}
		if (CrosshairPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(CrosshairPackage.CrosshairsBottom, ViewportCenter, Spread, CrosshairPackage.CrosshairsColor);
		}
		if (CrosshairPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(CrosshairPackage.CrosshairsCenter, ViewportCenter, Spread, CrosshairPackage.CrosshairsColor);
		}
	}
}

void ATFICharacterHUD::BeginPlay()
{
	Super::BeginPlay();
}

void ATFICharacterHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D TextureDrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}

void ATFICharacterHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

void ATFICharacterHUD::AddStopMenu()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && StopMenuClass)
	{
		QuickMenu = CreateWidget<UQuickMenuWidget>(PlayerController, StopMenuClass);
		QuickMenu->MenuSetup();
	}
}
