// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/BoxComponent.h>
#include "AddForceComponent.h"
#include "CPP_Pawn.h"
#include "CPP_Teleporter.generated.h"

UCLASS()
class JARDINMULTIPLE_API ACPP_Teleporter : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    ACPP_Teleporter();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditDefaultsOnly, Category = "Components")
    class UBoxComponent* BoxComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* TeleportPoint;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* BaseMesh;

private:


    UFUNCTION(BlueprintCallable)
    void OnOverlapStart(AActor* OtherActor);
};
