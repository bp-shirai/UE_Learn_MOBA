// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GenericTeamAgentInterface.h"

#include "CMinionBarrack.generated.h"

class ACMinion;
class APlayerStart;

UCLASS(Abstract)
class CRUNCH_API ACMinionBarrack : public AActor
{
    GENERATED_BODY()

public:
    ACMinionBarrack();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(EditAnywhere, Category = "Spawn")
    FGenericTeamId BarrackTeamID;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    float GroupSpawnInterval{5.f};

    UPROPERTY(EditAnywhere, Category = "Spawn")
    int32 MinionPerGroup{3};

    UPROPERTY()
    TArray<ACMinion*> MinionPool;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    AActor* Goal;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    TSubclassOf<ACMinion> MinionClass;

    UPROPERTY(EditAnywhere, Category = "Spawn")
    TArray<APlayerStart*> SpawnSpots;

    int32 NextSpawnSpotIndex{-1};

    const APlayerStart* GetNextPawnSpot();

    void SpawnNewGroup();
    void SpawnNewMinions(int32 Num);

    ACMinion* GetNextAvailableMinion() const;

    FTimerHandle SpawnIntervalTimerHandle;
};