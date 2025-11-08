// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "CGameplayAbility.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UCGameplayAbility();

protected:
    UAnimInstance* GetOwnerAnimInstance() const;

    TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius = 30.f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile,
                                                                bool bIgnoreSelf = true, bool bDrawDebug = false);
};
