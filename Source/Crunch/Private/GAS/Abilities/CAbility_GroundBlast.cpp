// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_GroundBlast.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

#include "GAS/AbilityTasks/CAbilityTask_PlayMontageWaitEvent.h"
#include "GAS/CGameplayTags.h"

UCAbility_GroundBlast::UCAbility_GroundBlast()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(FGameplayTagContainer(Tags::Ability::AbilityTwo));

    ActivationOwnedTags.AddTag(Tags::Stats::Aim);
}

void UCAbility_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
    {
        FGameplayTagContainer Tags(Tags::Ability::Combo_Damage);

        UCAbilityTask_PlayMontageWaitEvent* PlayMontage = UCAbilityTask_PlayMontageWaitEvent::CreatePlayMontageAndWaitProxyTags(this, NAME_None, GroundBlastMontage, Tags);
        // PlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnCancelled.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnCompleted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnEvent.AddDynamic(this, &ThisClass::HandleComboEvent);
        PlayMontage->ReadyForActivation();
    }
}

void UCAbility_GroundBlast::HandlePlayEnd(FGameplayTag EventTag, FGameplayEventData EventData)
{
    K2_EndAbility();
}

void UCAbility_GroundBlast::HandleComboEvent(FGameplayTag EventTag, FGameplayEventData EventData)
{
}
