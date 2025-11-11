// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Launched.h"
#include "Abilities/GameplayAbilityTypes.h"

#include "GAS/CGameplayTags.h"

UCAbility_Launched::UCAbility_Launched()
{
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
    FAbilityTriggerData TriggerData;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    TriggerData.TriggerTag    = Tags::Ability::Passive::Launch::Activate;

    AbilityTriggers.Add(TriggerData);
}

void UCAbility_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (K2_HasAuthority())
    {
        if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
        {
            const FVector PushVelocity = TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal;
            PushSelf(PushVelocity);
        }
    }

    K2_EndAbility();
}
