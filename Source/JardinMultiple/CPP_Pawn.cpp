// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Pawn.h"

// Sets default values
ACPP_Pawn::ACPP_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACPP_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACPP_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACPP_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

