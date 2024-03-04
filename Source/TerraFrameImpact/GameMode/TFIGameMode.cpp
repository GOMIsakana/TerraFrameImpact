// Fill out your copyright notice in the Description page of Project Settings.


#include "TFIGameMode.h"
#include "TerraFrameImpact/Character/TFICharacter.h"

ATFIGameMode::ATFIGameMode()
{

}

void ATFIGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATFIGameMode::RequestRespawn(ACharacter* ElimmitedCharacter, AController* ElimmedController)
{
	if (ElimmitedCharacter)
	{
		ElimmitedCharacter->Reset();
		ElimmitedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		RestartPlayerAtTransform(ElimmedController, ElimmitedCharacter->GetActorTransform());
	}
}
