// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "AddForceComponent.generated.h"

UENUM(BlueprintType)
enum class EGravityType : uint8
{
	UP         UMETA(DisplayName = "Up"),
	DOWN       UMETA(DisplayName = "Down"),
	LEFT       UMETA(DisplayName = "Left"),
	RIGHT      UMETA(DisplayName = "Right"),
	FORWARD    UMETA(DisplayName = "Forward"),
	BACKWARD   UMETA(DisplayName = "Backward"),
	CUSTOM    UMETA(DisplayName = "Custom"),
	NONE       UMETA(DisplayName = "None")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class JARDINMULTIPLE_API UAddForceComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAddForceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void ChangeGravity(EGravityType where);

	UPROPERTY(EditAnywhere, Category = "Gravity")
	EGravityType CurrentGravity = EGravityType::NONE;

private:
	UPROPERTY()
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Forces")
	float Force = 1080.0f;


	UPROPERTY(EditAnywhere, Category = "Gravity")
	FVector CustomGravity;

	FVector Gravity;


};
