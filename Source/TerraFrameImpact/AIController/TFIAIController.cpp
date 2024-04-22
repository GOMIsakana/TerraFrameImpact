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
	if (Character != nullptr && (CurrentLocation - CurrentTargetLocation).Size() > 2000.f)
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


	if (CurrentTarget != nullptr && !LineOfSightTo(CurrentTarget))
	{
		GetBlackboardComponent()->SetValueAsVector(TEXT("LastTargetLocation"), LastTargetLocation);
		GetBlackboardComponent()->ClearValue(TEXT("TargetPlayer"));
		CurrentTarget = nullptr;
	}

	FVector SelfLocation = Character->GetActorLocation();
	FVector GettingLastTargetLocation = FVector();
	if (GetBlackboardComponent()->IsVectorValueSet(TEXT("LastTargetLocation")))
	{
		GettingLastTargetLocation = GetBlackboardComponent()->GetValueAsVector(TEXT("LastTargetLocation"));
	}
	SelfLocation.Z = 0.f;
	GettingLastTargetLocation.Z = 0.f;

	if (FMath::Abs(SelfLocation.Size() - GettingLastTargetLocation.Size()) < 100.f || CurrentTarget != nullptr && LineOfSightTo(CurrentTarget))
	{
		GetBlackboardComponent()->ClearValue(TEXT("LastTargetLocation"));
	}

	// 获取所有玩家
	UGameplayStatics::GetAllActorsOfClass(this, ATFICharacter::StaticClass(), PlayerCharacterArray);
	// 当前锁定的目标为空，或当前锁定的目标处于倒地或者被歼灭的状态
	if (!bCurrentTargetValid())
	{
		CurrentTarget = nullptr;
		GetBlackboardComponent()->ClearValue(TEXT("TargetPlayer"));

		// 遍历可直视看到的玩家, 将它们添加到目标列表中
		TArray<ATFICharacter*> CanSeePlayers;
		for (AActor* Player : PlayerCharacterArray)
		{
			ATFICharacter* PlayerCharacter = Cast<ATFICharacter>(Player);
			if (PlayerCharacter->IsPlayerControlled())
			{
				// 当前锁定的目标可用, 且不处于倒地或被歼灭状态
				bool bCanSeeThisPlayer = LineOfSightTo(PlayerCharacter) && !PlayerCharacter->IsDying() && !PlayerCharacter->IsElimmed();
				if (bCanSeeThisPlayer)
				{
					CanSeePlayers.AddUnique(PlayerCharacter);
				}
			}
		}

		// 如果筛选到了可用的玩家, 选择随机的玩家作为目标
		if (CanSeePlayers.Num() > 0)
		{
			int32 RandomNum = FMath::RandRange(0, CanSeePlayers.Num() - 1);
			CurrentTarget = CanSeePlayers[RandomNum];
			// 将选定的玩家更新到黑板(BlackBroad)组件中
			GetBlackboardComponent()->SetValueAsObject(TEXT("TargetPlayer"), CurrentTarget);
		}
	}

	FVector CurrentLocation = Character->GetActorLocation();
	if (bCurrentTargetValid())
	{
		CurrentTargetLocation = CurrentTarget->GetActorLocation();
	}
	else
	{
		FVector RandomVector = UKismetMathLibrary::RandomUnitVector();
		RandomVector.Z = 0.f;
		CurrentTargetLocation = CurrentLocation + RandomVector * WanderRange;
	}

	GetBlackboardComponent()->SetValueAsVector(TEXT("TargetLocation"), CurrentTargetLocation);
	LastTargetLocation = CurrentTargetLocation;

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
	StopExecuteAI();
	Super::Destroyed();
}

FVector ATFIAIController::GetTargetHeadLocation()
{
	FVector HeadLocation = FVector();
	if (bCurrentTargetValid() && CurrentTarget->GetMesh() != nullptr)
	{
		const USkeletalMeshSocket* HeadSocket = CurrentTarget->GetMesh()->GetSocketByName("HeadSocket");
		if (HeadSocket != nullptr)
		{
			HeadLocation = HeadSocket->GetSocketLocation(CurrentTarget->GetMesh());
		}
	}
	return HeadLocation;
}

bool ATFIAIController::bCurrentTargetValid()
{
	return CurrentTarget != nullptr && !CurrentTarget->IsDying() && !CurrentTarget->IsElimmed();
}
