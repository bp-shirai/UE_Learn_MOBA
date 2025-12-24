// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Tornado.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CGameplayTags.h"

void UCAbility_Tornado::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayTornadoMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_Tornado);
        PlayTornadoMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayTornadoMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayTornadoMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayTornadoMontage->ReadyForActivation();

        if (K2_HasAuthority())
        {
            UAbilityTask_WaitGameplayEvent* WaitDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Generic_Damage);
            WaitDamageEvent->EventReceived.AddDynamic(this, &ThisClass::TornadoDamageEventReceived);
            WaitDamageEvent->ReadyForActivation();
        }

        UAbilityTask_WaitCancel* WaitCancel = UAbilityTask_WaitCancel::WaitCancel(this);
        WaitCancel->OnCancel.AddDynamic(this, &ThisClass::K2_EndAbility);
        WaitCancel->ReadyForActivation();

        UAbilityTask_WaitDelay* WaitTornadoTimeout = UAbilityTask_WaitDelay::WaitDelay(this, TornadoDuration);
        WaitTornadoTimeout->OnFinish.AddDynamic(this, &ThisClass::K2_EndAbility);
        WaitTornadoTimeout->ReadyForActivation();
    }
}

void UCAbility_Tornado::TornadoDamageEventReceived(FGameplayEventData EventData)
{
    if (K2_HasAuthority())
    {
        FGameplayAbilityTargetDataHandle TargetData = EventData.TargetData;
        BP_ApplyGameplayEffectToTarget(TargetData, GE_HitDamage, GetAbilityLevel());
        PushTargetsFromOwnerLocation(TargetData, HitPushSpeed);
    }
}
