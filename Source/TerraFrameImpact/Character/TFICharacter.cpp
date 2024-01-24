// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TFICharacter.h"

// Sets default values
ATFICharacter::ATFICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATFICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATFICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATFICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

