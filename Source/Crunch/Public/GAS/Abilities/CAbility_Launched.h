// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Launched.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Launched : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_Launched();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
