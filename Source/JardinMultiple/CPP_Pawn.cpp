 // Fill out your copyright notice in the Description page of Project Settings.

#include "CPP_Pawn.h"

//Jai pas trouve le utils
static float WrapAxis(float Value, float Min, float Max)
{
	const float Range = Max - Min;
	return Min + FMath::Fmod(FMath::Fmod(Value - Min, Range) + Range, Range);
}


// Sets default values
ACPP_Pawn::ACPP_Pawn()
{
	CreateComponent();
}

void ACPP_Pawn::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = false;

	GetWorldTimerManager().SetTimer(
		CheckTimerHandle,
		this,
		&ACPP_Pawn::CustomTimer,
		0.01f,
		true
	);

	StartPos = GetActorLocation();

	SetupComponent();
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ACPP_Pawn::CreateComponent()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	RootRotation = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootRotation"));
	RootRotation->SetupAttachment(Mesh);

	CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	CapsuleCollider->SetupAttachment(RootRotation);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(RootRotation);

	GrabPoint = CreateDefaultSubobject<USceneComponent>(TEXT("GrabPoint"));
	GrabPoint->SetupAttachment(CameraComp);

	GrabPoint->SetRelativeLocation(FVector(GrabDistance, 0.f, 0.f));
}

void ACPP_Pawn::CheckGrabPosition()
{
	if (IsTakingItem)
	{
		FVector newLocation = GrabPoint->GetComponentLocation();
		TakingItem->SetActorLocation(newLocation);
	}

}

void ACPP_Pawn::SetupComponent()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(MyMappingContext, 0);
		}
	}
	

	FVector Min, Max;
	Mesh->GetLocalBounds(Min, Max);
	Mesh->SetSimulatePhysics(IsPhysicSimulated);

	const FVector BoxExtent = (Max - Min) * 0.5f;
	const FVector CapsuleCenter = (Max + Min) * 0.5f;

	CapsuleCollider->SetCapsuleSize(BoxExtent.X , BoxExtent.Z);
	CapsuleCollider->SetRelativeLocation(CapsuleCenter);
	CapsuleCollider->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleCollider->SetCollisionObjectType(ECC_Pawn);
	CapsuleCollider->SetCollisionResponseToAllChannels(ECR_Block);
}

void ACPP_Pawn::RotatePlayer(FRotator Rotator)
{
	SetActorRotation(RootRotation->GetRelativeRotation() + Rotator);
}

FVector ACPP_Pawn::IsFarOffset()
{
	if (GetActorLocation().X >= StartPos.X + OffSet.X)
		Direction.X = -1;
	else if (GetActorLocation().X <= StartPos.X - OffSet.X)
		Direction.X = 1;

	if (GetActorLocation().Y >= StartPos.Y + OffSet.Y)
		Direction.Y = -1;
	else if (GetActorLocation().Y <= StartPos.Y - OffSet.Y)
		Direction.Y = 1;

	if (GetActorLocation().Z >= StartPos.Z + OffSet.Z)
		Direction.Z = -1;
	else if (GetActorLocation().Z <= StartPos.Z - OffSet.Z)
		Direction.Z = 1;

	return Direction;
}

void ACPP_Pawn::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(playerInputComponent))
	{
		EnhancedInput->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ACPP_Pawn::MoveCallback);
		EnhancedInput->BindAction(MovementAction, ETriggerEvent::Completed, this, &ACPP_Pawn::StopMovement);
		EnhancedInput->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ACPP_Pawn::TurnCamera);
		EnhancedInput->BindAction(LookUpDownAction, ETriggerEvent::Triggered, this, &ACPP_Pawn::LookUpCamera);
		EnhancedInput->BindAction(ChangeWallAction, ETriggerEvent::Started, this, &ACPP_Pawn::ChangeWall);
		EnhancedInput->BindAction(InteractItemAction, ETriggerEvent::Started, this, &ACPP_Pawn::InteractItem);
	}
}

void ACPP_Pawn::InteractItem()
{
	if (IsTakingItem == false)
		RayInteractItem();
	else
		ThrowItem();
}

void ACPP_Pawn::ChangeWall()
{
	Ray();
}

void ACPP_Pawn::MoveCallback(const FInputActionValue& Value)
{
	FVector2D InputVector = Value.Get<FVector2D>();

	Direction = RootRotation->GetForwardVector() * InputVector.X + RootRotation->GetRightVector() * InputVector.Y;


	Direction.Normalize();

	Mesh->AddForce(Direction * Speed * Mesh->GetMass(),NAME_None,true);
}

void ACPP_Pawn::StopMovement(const FInputActionValue& Value)
{
	Direction = FVector::ZeroVector;
}

void ACPP_Pawn::TurnCamera(const FInputActionValue& Value)
{
	if (!bIsRotating) {
		float Input = Value.Get<float>();

		FQuat ActualRotation = RootRotation->GetComponentQuat();
		float AngleRad = Input * CamSensitivityX;
		FQuat DeltaQuat(-GravtityDir, AngleRad);
		RootRotation->SetRelativeRotation(DeltaQuat * ActualRotation);
	}
	
}

void ACPP_Pawn::LookUpCamera(const FInputActionValue& Value)
{
	if (!bIsRotating) {
		float Input = Value.Get<float>();

		FRotator Rot = CameraComp->GetRelativeRotation();

		Rot.Pitch = FMath::Clamp(
			Rot.Pitch + Input * CamSensitivityY,
			-60.0f,
			60.0f
		);

		CameraComp->SetRelativeRotation(Rot);
	}
}

void ACPP_Pawn::Ray()
{
	FVector start = GetActorLocation();
	FVector forward = CameraComp->GetForwardVector();
	start = FVector(start.X + (forward.X * 87), start.Y + (forward.Y * 87), start.Z + (forward.Z * 87));

	FVector end = start + (forward * 50);
	FHitResult hit;
	
	if (GetWorld())
	{
		bool actorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Pawn, FCollisionQueryParams(),FCollisionResponseParams());
		if (actorHit && hit.GetComponent())
		{
			if (hit.GetComponent()->ComponentHasTag("Wall") && IsTakingItem == false)
			{


				ChangeWalkSideState(hit.GetComponent());
			}
		}
	}
}

void ACPP_Pawn::RayInteractItem()
{
	FVector start = GetActorLocation();
	FVector forward = CameraComp->GetForwardVector();
	start = FVector(start.X + (forward.X * 100), start.Y + (forward.Y * 100), start.Z + (forward.Z * 100));

	FVector end = start + (forward * 100);
	FHitResult hit;

	if (GetWorld())
	{
		bool actorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Pawn, FCollisionQueryParams(), FCollisionResponseParams());

		if (actorHit && hit.GetActor())
		{

			if (hit.GetActor()->ActorHasTag("Interactable"))
			{
				if (IsTakingItem == false)
				{

					TakeItem(hit.GetActor());
				}
			}
		}
	}
}

bool ACPP_Pawn::IsGrounded()
{
	FVector start = RootRotation->GetComponentLocation();
	FVector down = -RootRotation->GetUpVector();

	start += down * 87;

	FVector end = start + (down * 50);
	FHitResult hit;

	if (GetWorld())
	{
		bool actorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Pawn, FCollisionQueryParams(), FCollisionResponseParams());

		if (actorHit)
		{
			return true;
		}
	}

	return false;
}

void ACPP_Pawn::TakeItem(AActor* hit)
{
	IsTakingItem = true;
	TakingItem = hit;

	UAddForceComponent* ForceComp = hit->FindComponentByClass<UAddForceComponent>();
	//UPrimitiveComponent* PrimComp = hit->FindComponentByClass<UPrimitiveComponent>();

	if (ForceComp)
	{
		MemTakingObjectGravityType = ForceComp->CurrentGravity;
		ForceComp->ChangeGravity(EGravityType::NONE);
		//PrimComp->SetSimulatePhysics(false);
		UGameplayStatics::PlaySound2D(this, WallChangedSound);
		
	}
}


void ACPP_Pawn::ThrowItem()
{
	IsTakingItem = false;

	UAddForceComponent* ForceComp = TakingItem->FindComponentByClass<UAddForceComponent>();


	if (ForceComp)
	{
		ForceComp->ChangeGravity(MemTakingObjectGravityType);
	}

	UGameplayStatics::PlaySound2D(this, ThrowCubeSound);
}

void ACPP_Pawn::ChangeWalkSideState(UPrimitiveComponent* hit)
{
	Mesh->SetSimulatePhysics(false);

	bool WallChanged = false;

	if (hit->ComponentHasTag("Right"))
	{
		if (AuthorizedSwitchSide[(int32)CurrentSide].Contains(EWalkSide::RIGHT))
		{
			CurrentSide = EWalkSide::RIGHT;
			GravtityDir = FVector(1, 0, 0);
			WallChanged = true;
		}
	}
	else if (hit->ComponentHasTag("Left"))
	{
		if (AuthorizedSwitchSide[(int32)CurrentSide].Contains(EWalkSide::LEFT))
		{
			CurrentSide = EWalkSide::LEFT;

			GravtityDir = FVector(-1, 0, 0);

			WallChanged = true;

		}
	}
	else if (hit->ComponentHasTag("Forward"))
	{
		if (AuthorizedSwitchSide[(int32)CurrentSide].Contains(EWalkSide::FORWARD))
		{
			CurrentSide = EWalkSide::FORWARD;

			GravtityDir = FVector(0, 1, 0);
			WallChanged = true;

		}
	}
	else if (hit->ComponentHasTag("Backward"))
	{
		if (AuthorizedSwitchSide[(int32)CurrentSide].Contains(EWalkSide::BACKWARD))
		{
			CurrentSide = EWalkSide::BACKWARD;
			GravtityDir = FVector(0, -1, 0);

			WallChanged = true;

		}
	}
	else if (hit->ComponentHasTag("Ceil"))
	{
		if (AuthorizedSwitchSide[(int32)CurrentSide].Contains(EWalkSide::CEIL))
		{
			CurrentSide = EWalkSide::CEIL;

			GravtityDir = FVector(0, 0, 1);
			WallChanged = true;


		}
	}
	else if (hit->ComponentHasTag("Ground"))
	{
		if (AuthorizedSwitchSide[(int32)CurrentSide].Contains(EWalkSide::GROUND))
		{
			CurrentSide = EWalkSide::GROUND;

			GravtityDir = FVector(0, 0, -1);

			WallChanged = true;

		}
	}

	if(WallChanged)
	{
		StartRotation = RootRotation->GetComponentQuat();
		TargetRotation = FRotationMatrix::MakeFromZY(-GravtityDir, RootRotation->GetForwardVector()).ToQuat();

		bIsRotating = true;

		UGameplayStatics::PlaySound2D(this, WallChangedSound);
	}


	Mesh->SetSimulatePhysics(true);
}

void ACPP_Pawn::CheckWorldBounds()
{
	FVector Location = GetActorLocation();

	const FVector Min = WorldCenter - WorldExtent;
	const FVector Max = WorldCenter + WorldExtent;

	FVector WrappedLocation;
	WrappedLocation.X = WrapAxis(Location.X, Min.X, Max.X);
	WrappedLocation.Y = WrapAxis(Location.Y, Min.Y, Max.Y);
	WrappedLocation.Z = WrapAxis(Location.Z, Min.Z, Max.Z);

	if (!Location.Equals(WrappedLocation, 0.1f))
	{
		TeleportTo(WrappedLocation, GetActorRotation(), false, true);
	}
}

void ACPP_Pawn::AddDirectionalGravity(float dt)
{	
	if (gravityenabled) 
	{

		if (!IsGrounded()) {

			Mesh->AddForce(GravtityDir * GravityForce * Mesh->GetMass() * dt, NAME_None, true);

			if (IsFallingSoundAlreadyPlayed == false)
			{
				FallingAudioComponent = UGameplayStatics::SpawnSound2D(
   				this,
    			FallingSound
				);

				IsFallingSoundAlreadyPlayed = true;
				FallingAudioComponent->Play();
			}
			else 
			{
				//UGameplayStatics::PlaySound2D(this, FallingSound);
				IsFallingSoundAlreadyPlayed = false;
				FallingAudioComponent->Stop();
			}
		}
	}
}
void ACPP_Pawn::RotationHandling(){
	if (bIsRotating)
	{
		FQuat Current = RootRotation->GetComponentQuat();

		FQuat NewQuat = FQuat::Slerp(Current, TargetRotation, 0.01f * RotationInterpSpeed);
		NewQuat.Normalize();

		RootRotation->SetWorldRotation(NewQuat);

		// Stop condition
		if (Current.Equals(TargetRotation, 0.01f))
		{
			RootRotation->SetWorldRotation(TargetRotation);
			bIsRotating = false;
		}
	}
}

void ACPP_Pawn::CustomTimer() {
	CheckGrabPosition();
	RotationHandling();
}