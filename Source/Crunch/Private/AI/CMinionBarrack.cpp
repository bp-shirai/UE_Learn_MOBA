// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/CMinionBarrack.h"

#include "GameFramework/PlayerStart.h"

#include "AI/CMinion.h"
#include "GAS/CAbilitySystemStatics.h"

ACMinionBarrack::ACMinionBarrack()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACMinionBarrack::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &ThisClass::SpawnNewGroup, GroupSpawnInterval, true);
	}
}

void ACMinionBarrack::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ACMinionBarrack::SpawnNewMinions(int32 Num)
{
    for (int32 i = 0; i < Num; i++)
    {
        FTransform SpawnTransform;
        if (const APlayerStart* NextSpawnSpot = GetNextPawnSpot())
        {
            SpawnTransform = NextSpawnSpot->GetActorTransform();
        }

        ACMinion* NewMinion = GetWorld()->SpawnActorDeferred<ACMinion>(MinionClass, SpawnTransform, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
        if (NewMinion)
        {
            NewMinion->SetGenericTeamId(BarrackTeamID);
            NewMinion->FinishSpawning(SpawnTransform);
            MinionPool.Add(NewMinion);
        }
    }
}

const APlayerStart* ACMinionBarrack::GetNextPawnSpot()
{
    if (SpawnSpots.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("%s, SpawnSpots is empty"), *GetName());
        return nullptr;
    }

    if (++NextSpawnSpotIndex >= SpawnSpots.Num())
    {
        NextSpawnSpotIndex = 0;
    }

    return SpawnSpots[NextSpawnSpotIndex];
}

void ACMinionBarrack::SpawnNewGroup()
{
    int32 i = MinionPerGroup;

    while (i > 0)
    {
		FTransform SpawnTransform = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextPawnSpot())
		{
			SpawnTransform = NextSpawnSpot->GetActorTransform();
		}

		ACMinion* NextAvailableMinion = GetNextAvailableMinion();
		if (!NextAvailableMinion) break;

		NextAvailableMinion->SetActorTransform(SpawnTransform);
		NextAvailableMinion->Activate();

		i--;
    }

	SpawnNewMinions(i);
}

ACMinion* ACMinionBarrack::GetNextAvailableMinion() const
{
	for(ACMinion* Minion : MinionPool)
	{
		if(!Minion->IsActive())
		{
			return Minion;
		}
	}
   
	return nullptr;
}
