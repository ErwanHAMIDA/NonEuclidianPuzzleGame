// Fill out your copyright notice in the Description page of Project Settings.


#include "AddForceComponent.h"

// Sets default values for this component's properties
UAddForceComponent::UAddForceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UAddForceComponent::BeginPlay()
{
	Super::BeginPlay();
	MeshComp = Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent());

	ChangeGravity(CurrentGravity);
}


// Called every frame
void UAddForceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//maybe avoid on TickComponent --> can't use GetWorldTimerManager

	MeshComp->AddForce(Gravity * Force * MeshComp->GetMass() * DeltaTime);
}

void UAddForceComponent::ChangeGravity(EGravityType where)
{
	CurrentGravity = where;

	switch (where)
	{
	case(EGravityType::RIGHT):
		Gravity = FVector::RightVector;
		break;
	case(EGravityType::LEFT):
		Gravity = FVector::LeftVector;
		break;
	case(EGravityType::UP):
		Gravity = FVector::UpVector;
		break;
	case(EGravityType::DOWN):
		Gravity = FVector::DownVector;
		break;
	case(EGravityType::FORWARD):
		Gravity = FVector::ForwardVector;
		break;
	case(EGravityType::BACKWARD):
		Gravity = FVector::BackwardVector;
		break;
	case(EGravityType::CUSTOM):
		Gravity = CustomGravity;
		break;
	case(EGravityType::NONE):
		Gravity = FVector::ZeroVector;
		break;
	}
}

