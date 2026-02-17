// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Teleporter.h"

// Sets default values
ACPP_Teleporter::ACPP_Teleporter()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    RootComponent = BaseMesh;

    BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("Collider"));
    BoxComp->SetupAttachment(BaseMesh);

    TeleportPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TPPoint"));
    TeleportPoint->SetupAttachment(BaseMesh);

    
}

// Called when the game starts or when spawned
void ACPP_Teleporter::BeginPlay()
{
    Super::BeginPlay();

    BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BoxComp->SetCollisionObjectType(ECC_WorldDynamic);
    BoxComp->SetCollisionResponseToAllChannels(ECR_Ignore);

    BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    BoxComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    BoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

    BoxComp->SetGenerateOverlapEvents(true);
}

void ACPP_Teleporter::OnOverlapStart(AActor* OtherActor)
{
    if (!OtherActor || OtherActor == this) return;

    ACPP_Pawn* player = Cast<ACPP_Pawn>(OtherActor);
    
    UAddForceComponent* MovableCube =
        OtherActor->FindComponentByClass<UAddForceComponent>();

    if (player)
    {
        player->TeleportTo(TeleportPoint->GetComponentLocation(), player->GetActorRotation());
    }

    if (MovableCube)
    {
        if (OtherActor->ActorHasTag("Interactable"))
            OtherActor->SetActorLocation(TeleportPoint->GetComponentLocation());
    }
}

