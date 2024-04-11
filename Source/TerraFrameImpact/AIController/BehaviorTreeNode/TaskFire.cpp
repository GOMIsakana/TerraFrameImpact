// Fill out your copyright notice in the Description page of Project Settings.


#include "TaskFire.h"
#include "TerraFrameImpact/Character/TFIAICharacter.h"
#include "TerraFrameImpact/Components/TFICombatComponent.h"
#include "AIController.h"

UTaskFire::UTaskFire()
{
	NodeName = TEXT("CharacterFire");
}

EBTNodeResult::Type UTaskFire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	if (OwnerComp.GetAIOwner() == nullptr) return EBTNodeResult::Failed;

	ATFIAICharacter* Character = Cast<ATFIAICharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Character == nullptr) return EBTNodeResult::Failed;
	if (Character->IsDying() || Character->IsElimmed()) return EBTNodeResult::Failed;

	Character->GetCombatComponent()->SetFiring(true);
	if (Character->GetCombatComponent()->CanFire())
	{
		Character->PlayFireMontage(false);
	}
	Character->GetCombatComponent()->SetFiring(false);

	return EBTNodeResult::Succeeded;
}