#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Normal UMETA(DisplayName = "常态"),
	ECS_Slide UMETA(DisplayName = "滑铲"),
	ECS_BulletJump UMETA(DisplayName = "子弹跳"),
	ECS_Fire UMETA(DisplayName = "开火"),
	ECS_Reload UMETA(DisplayName = "装填"),

	ECS_MAX UMETA(DisplayName = "DefaultMAX")
};