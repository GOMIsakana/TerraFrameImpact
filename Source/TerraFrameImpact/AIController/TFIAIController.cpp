// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TerraFrameImpact/Character/TFIAICharacter.h"
#include "TerraFrameImpact/Components/TFICombatComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/SkeletalMeshSocket.h"

void ATFIAIController::BeginPlay()
{
	Super::BeginPlay();
	StartExecuteAI();
	// 可能会因为网络问题出现找不到另一个玩家的情况
}

void ATFIAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	UGameplayStatics::GetAllActorsOfClass(this, ATFICharacter::StaticClass(), PlayerCharacterArray);
	Character = Character == nullptr ? Cast<ATFIAICharacter>(GetCharacter()) : Character;
	if (Character == nullptr) return;
	FVector CurrentLocation = Character->GetActorLocation();

	CurrentLocation.Z = 0.f;
	if (Character != nullptr && (CurrentLocation - CurrentTargetLocation).Size() > 2500.f)
	{
		Character->GetCombatComponent()->SetDashButtonPressed(true);
	}
	else
	{
		Character->GetCombatComponent()->SetDashButtonPressed(false);
	}
}

void ATFIAIController::ExecuteAI()
{
	Character = Character == nullptr ? Cast<ATFIAICharacter>(GetCharacter()) : Character;
	if (Character == nullptr) return;


	UGameplayStatics::GetAllActorsOfClass(this, ATFICharacter::StaticClass(), PlayerCharacterArray);
	if (CurrentTarget != nullptr && !LineOfSightTo(CurrentTarget))
	{
		CurrentTarget = nullptr;
		GetBlackboardComponent()->ClearValue(TEXT("TargetPlayer"));
		// MoveToLocation(TargetLocation, 50.f);
	}

	if (CurrentTarget == nullptr || CurrentTarget->IsDying() || CurrentTarget->IsElimmed())
	{
		TArray<ATFICharacter*> CanSeePlayers;
		for (AActor* Player : PlayerCharacterArray)
		{
			ATFICharacter* PlayerCharacter = Cast<ATFICharacter>(Player);
			if (PlayerCharacter->IsPlayerControlled())
			{
				bool bCanSeeThisPlayer = LineOfSightTo(PlayerCharacter) && !PlayerCharacter->IsDying() && !PlayerCharacter->IsElimmed();
				if (bCanSeeThisPlayer)
				{
					CanSeePlayers.AddUnique(PlayerCharacter);
				}
			}
		}

		if (CanSeePlayers.Num() > 0)
		{
			int32 RandomNum = FMath::RandRange(0, CanSeePlayers.Num() - 1);
			CurrentTarget = CanSeePlayers[RandomNum];
			GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), CurrentTarget);
		}
	}

	Character = Character == nullptr ? Cast<ATFIAICharacter>(GetCharacter()) : Character;
	if (Character == nullptr) return;
	FVector CurrentLocation = Character->GetActorLocation();
	if (CurrentTarget != nullptr)
	{
		// SetFocus(CurrentTarget);
		// MoveToActor(CurrentTarget);
		CurrentTargetLocation = CurrentTarget->GetActorLocation();
	}
	else
	{
		// ClearFocus(EAIFocusPriority::Gameplay);
		FVector RandomVector = UKismetMathLibrary::RandomUnitVector();
		RandomVector.Z = 0.f;
		CurrentTargetLocation = CurrentLocation + RandomVector * WanderRange;
		// MoveToLocation(TargetLocation, 50.f);
	}

	GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), CurrentTargetLocation);

	if (Character->IsDying() || Character->IsElimmed())
	{
		Character->Destroy();
		Destroy();
	}
}

void ATFIAIController::StartExecuteAI()
{
	if (!HasAuthority()) return;
	GetWorld()->GetTimerManager().SetTimer(
		ExecuteAITimer,
		this,
		&ATFIAIController::ExecuteAI,
		ExecuteAIDelay,
		true,
		ExecuteAIDelay
	);
	if (BehaviorTree != nullptr)
	{
		RunBehaviorTree(BehaviorTree);
	}
}

void ATFIAIController::StopExecuteAI()
{
	GetWorld()->GetTimerManager().ClearTimer(ExecuteAITimer);
}

void ATFIAIController::Destroyed()
{
	Super::Destroyed();

	StopExecuteAI();
	GetBlackboardComponent()->ClearValue(TEXT("TargetPlayer"));
}

FVector ATFIAIController::GetTargetHeadLocation()
{
	if (CurrentTarget == nullptr) return FVector();
	const USkeletalMeshSocket* HeadSocket = CurrentTarget->GetMesh()->GetSocketByName("HeadSocket");
	FVector HeadLocation = FVector();
	if (HeadSocket != nullptr)
	{
		HeadLocation = HeadSocket->GetSocketLocation(CurrentTarget->GetMesh());
	}
	return HeadLocation;
}
