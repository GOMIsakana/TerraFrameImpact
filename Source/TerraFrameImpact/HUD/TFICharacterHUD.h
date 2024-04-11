// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TFICharacterHUD.generated.h"

USTRUCT(BlueprintType)
struct FCrosshairPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;
	UTexture2D* CrosshairsLeft;
	UTexture2D* CrosshairsRight;
	UTexture2D* CrosshairsTop;
	UTexture2D* CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairsColor = FLinearColor::White;
};

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFICharacterHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;
	void AddCharacterOverlay();
	void AddStopMenu();
	UPROPERTY(VisibleAnywhere, Category = "玩家状态")
	class UCharacterOverlay* CharacterOverlay;
	UPROPERTY(VisibleAnywhere, Category = "玩家状态")
	class UQuickMenuWidget* QuickMenu;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class APlayerController* OwningPlayer;

	FCrosshairPackage CrosshairPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere, Category = "玩家状态")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY(EditAnywhere, Category = "玩家状态")
	TSubclassOf<class UUserWidget> StopMenuClass;

public:
	FORCEINLINE void SetCrosshairPackage(const FCrosshairPackage& Package) { CrosshairPackage = Package; }
};
