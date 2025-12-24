// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Freeze.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_NetworkSyncPoint.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CGameplayTags.h"
#include "GAS/TargetActor/CTargetActor_GroundPick.h"
#include "GameplayEffectTypes.h"
#include "Templates/Casts.h"

UCAbility_Freeze::UCAbility_Freeze()
{
    ActivationOwnedTags.AddTag(Tags::Stats::Aim);
}

void UCAbility_Freeze::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayTargetingMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_FreezeTargeting);
        PlayTargetingMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayTargetingMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayTargetingMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayTargetingMontage->ReadyForActivation();

        UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
        WaitTargetData->ValidData.AddDynamic(this, &ThisClass::TargetReceived);
        WaitTargetData->Cancelled.AddDynamic(this, &ThisClass::TargetingCancelled);
        WaitTargetData->ReadyForActivation();

        AGameplayAbilityTargetActor* TargetActor;
        WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);

        ACTargetActor_GroundPick* GroundPick = CastChecked<ACTargetActor_GroundPick>(TargetActor);
        GroundPick->SetTargetAreaRadius(TargetingRadius);
        GroundPick->SetTargetTraceRange(TargetingRange);
        GroundPick->SetShouldDrawDebug(ShouldDrawDebug());

        WaitTargetData->FinishSpawningActor(this, TargetActor);
    }
}

void UCAbility_Freeze::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (K2_HasAuthority())
    {
        BP_ApplyGameplayEffectToTarget(TargetData, GE_Damage, GetAbilityLevel());
    }

    FGameplayCueParameters Params;
    Params.Location     = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetData, 1).ImpactPoint;
    Params.RawMagnitude = TargetingRadius;
    GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(FreezingTargetCueTag, Params);

    PlayMontageLocally(AM_FreezeCast);
    K2_EndAbility();
}

void UCAbility_Freeze::TargetingCancelled(const FGameplayAbilityTargetDataHandle& TargetData)
{
    K2_EndAbility();
}
