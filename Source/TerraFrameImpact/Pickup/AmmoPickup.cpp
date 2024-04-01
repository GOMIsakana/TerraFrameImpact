// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"
#include "TerraFrameImpact/Character/TFICharacter.h"
#include "TerraFrameImpact/Components/TFICombatComponent.h"
#include "TerraFrameImpact/PlayerController/TFIPlayerController.h"

void AAmmoPickup::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ATFICharacter* Character = Cast<ATFICharacter>(OtherActor);
	if (Character != nullptr && Character->IsPlayerControlled())
	{
		ATFIPlayerController* Controller = Cast<ATFIPlayerController>(Character->GetController());
		UTFICombatComponent* Combat = Character->GetCombatComponent();
		if (Combat != nullptr && Combat->PickupAmmo(WeaponType, AmmoAmount))
		{
			Destroy();
			if (Controller != nullptr)
			{
				Controller->AddNewPickupNotify(ItemName, AmmoAmount);
			}
		}
	}
}
