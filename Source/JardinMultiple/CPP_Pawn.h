// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include <EnhancedInputLibrary.h>
#include <Components/CapsuleComponent.h>
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "AddForceComponent.h"




#include "CPP_Pawn.generated.h"

UCLASS()
class JARDINMULTIPLE_API ACPP_Pawn : public APawn
{
	GENERATED_BODY()

	enum class EWalkSide
	{
		GROUND,
		CEIL,
		RIGHT,
		LEFT,
		FORWARD,
		BACKWARD
	};
	
	EWalkSide CurrentSide = EWalkSide::GROUND;

public:
	// Sets default values for this pawn's properties
	ACPP_Pawn();

	void CreateComponent();

	FTimerHandle CheckTimerHandle;

	void CheckGrabPosition();

	UPROPERTY(EditAnywhere)
	USoundBase* WallChangedSound;

	UPROPERTY(EditAnywhere)
	USoundBase* ThrowCubeSound;
	
	UPROPERTY(EditAnywhere)
    USoundBase* FallingSound;
    
    UPROPERTY(EditAnywhere)
    USoundBase* TeleportSound;

	UPROPERTY()
	UAudioComponent* FallingAudioComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetupComponent();

	UPROPERTY(EditAnywhere)
	class UCameraComponent* CameraComp;

	bool IsFallingSoundAlreadyPlayed = false;

public:	
	// Called every frame
	FVector IsFarOffset();
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RootRotation;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CapsuleCollider;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSettings)
	float Speed = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSettings)
	float GravityForce = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerSettings)
	bool IsPhysicSimulated = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float CamSensitivityX = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float CamSensitivityY = 2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CameraSettings)
	float CamSpeedLag = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OffSet = FVector(100, 100, 100);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputMappingContext* MyMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* MovementAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* LookAroundAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* LookUpDownAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* ChangeWallAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Input)
	UInputAction* InteractItemAction;

	UPROPERTY(BlueprintReadWrite, Category = "Input")
	bool bSwapMouseAxes = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab")
	class USceneComponent* GrabPoint;

	UPROPERTY(EditAnywhere, Category = "Grab")
	float GrabDistance = 150.0f;

	UPROPERTY(EditAnywhere, Category = "rotation speed")
	float RotationInterpSpeed = 5.f;




	bool IsTakingItem = false;

	AActor* TakingItem;

	EGravityType MemTakingObjectGravityType;

	void ChangeWall();
	void SetupPlayerInputComponent(UInputComponent* playerInputComponent);
	void InteractItem();
	void MoveCallback(const FInputActionValue& Value);
	void StopMovement(const FInputActionValue& Value);

	void TurnCamera(const FInputActionValue& Value);

	void LookUpCamera(const FInputActionValue& Value);

	void Ray();

	void RayInteractItem();

	bool IsGrounded();

	void TakeItem(AActor* hit);

	void ThrowItem();

	void ChangeWalkSideState(UPrimitiveComponent* hit);

	UFUNCTION(BlueprintCallable)
	void CheckWorldBounds();

	UFUNCTION(BlueprintCallable)
	void AddDirectionalGravity(float dt);
	void RotationHandling();
	void CustomTimer();


	TArray<TArray<EWalkSide>> AuthorizedSwitchSide = {
		{ EWalkSide::RIGHT, EWalkSide::LEFT, EWalkSide::FORWARD, EWalkSide::BACKWARD},
		{  EWalkSide::RIGHT, EWalkSide::LEFT, EWalkSide::FORWARD, EWalkSide::BACKWARD},
		{  EWalkSide::GROUND, EWalkSide::CEIL, EWalkSide::FORWARD, EWalkSide::BACKWARD},
		{  EWalkSide::GROUND, EWalkSide::CEIL, EWalkSide::FORWARD, EWalkSide::BACKWARD},
		{  EWalkSide::RIGHT, EWalkSide::LEFT, EWalkSide::GROUND, EWalkSide::CEIL},
		{  EWalkSide::RIGHT, EWalkSide::LEFT, EWalkSide::GROUND, EWalkSide::CEIL}
	};

	UFUNCTION(BlueprintCallable)
	void RotatePlayer(FRotator Rotator);

	UPROPERTY(EditAnywhere, Category = "World Wrap")
	FVector WorldCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "World Wrap")
	FVector WorldExtent = FVector(2000.f, 3000.f, 2000.f);


private:
	FVector StartPos;
	FVector Direction = FVector(0, 0, 0);
	FVector GravtityDir = FVector(0, 0, -1);
	bool gravityenabled = true;

	// Rotation interpolation
	FQuat StartRotation;
	FQuat TargetRotation;
	bool bIsRotating = false;

};
