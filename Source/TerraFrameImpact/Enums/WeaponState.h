#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Unpicked UMETA(DisplayName = "未拾取"),
	EWS_HasOwner UMETA(DisplayName = "已有主"),
	EWS_Dropped UMETA(DisplayName = "已丢弃"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};