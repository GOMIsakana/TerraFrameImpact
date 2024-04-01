// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractDoor.h"

AInteractDoor::AInteractDoor()
{
	MoveTimelineComponent = CreateDefaultSubobject<UTimelineComponent>(TEXT("移动时间轴"));
}

void AInteractDoor::Interact()
{
	MoveTimeLocationTrack.BindDynamic(this, &AInteractDoor::UpdateMoveLocation);
	MoveTimeRotationTrack.BindDynamic(this, &AInteractDoor::UpdateMoveRotation);
	MoveTimeScaleTrack.BindDynamic(this, &AInteractDoor::UpdateMoveScale);
	if (MoveTimelineComponent)
	{
		if (MoveLocationCurve)
		{
			MoveTimelineComponent->AddInterpVector(MoveLocationCurve, MoveTimeLocationTrack);
		}
		if (MoveRotationCurve)
		{
			MoveTimelineComponent->AddInterpVector(MoveRotationCurve, MoveTimeRotationTrack);
		}
		if (MoveScaleCurve)
		{
			MoveTimelineComponent->AddInterpVector(MoveScaleCurve, MoveTimeScaleTrack);
		}
		MoveTimelineComponent->PlayFromStart();
	}
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		DisableInteract();
	}
}

void AInteractDoor::UpdateMoveLocation(FVector Location)
{
	StaticMesh->SetRelativeLocation(TargetTransform.GetLocation() * Location);
}

void AInteractDoor::UpdateMoveRotation(FVector Rotation)
{
	StaticMesh->SetRelativeRotation(Rotation.Rotation().Quaternion() * TargetTransform.GetRotation());
}

void AInteractDoor::UpdateMoveScale(FVector Scale)
{
	StaticMesh->SetRelativeScale3D(Scale * TargetTransform.GetScale3D());
}
