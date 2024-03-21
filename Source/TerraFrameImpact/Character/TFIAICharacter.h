// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TFICharacter.h"
#include "TFIAICharacter.generated.h"

/**
 * 
 */
UCLASS()
class TERRAFRAMEIMPACT_API ATFIAICharacter : public ATFICharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void KnockDown() override; 

	virtual void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> DefaultWeaponClass;

private:

	ATFIPlayerController* LastAttackerController;
};
