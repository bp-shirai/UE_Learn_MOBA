// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_GroundBlast.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

#include "GAS/AbilityTasks/CAbilityTask_PlayMontageWaitEvent.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CGameplayTags.h"
#include "GAS/TargetActor/CTargetActor_GroundPick.h"
#include "GameplayEffectTypes.h"
#include "UObject/UnrealNames.h"

UCAbility_GroundBlast::UCAbility_GroundBlast()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
    ActivationOwnedTags.AddTag(Tags::Stats::Aim);
}

void UCAbility_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, TargetingMontage);
        // PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontageTask->OnCancelled.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontageTask->OnCompleted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontageTask->ReadyForActivation();

        UAbilityTask_WaitTargetData* WaitTargetDataTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
        WaitTargetDataTask->ValidData.AddDynamic(this, &ThisClass::TargetConfirmed);
        WaitTargetDataTask->Cancelled.AddDynamic(this, &ThisClass::TargetCanceled);
        WaitTargetDataTask->ReadyForActivation();

        AGameplayAbilityTargetActor* TargetActor;
        WaitTargetDataTask->BeginSpawningActor(this, TargetActorClass, TargetActor);

        if (ACTargetActor_GroundPick* GroundPickActor = Cast<ACTargetActor_GroundPick>(TargetActor))
        {
            GroundPickActor->SetTargetAreaRadius(TargetAreaRadius);
            GroundPickActor->SetTargetTraceRange(TargetTraceRange);
            GroundPickActor->SetShouldDrawDebug(ShouldDrawDebug());
        }

        WaitTargetDataTask->FinishSpawningActor(this, TargetActor);
    }
}

void UCAbility_GroundBlast::HandlePlayEnd()
{
    K2_EndAbility();
}

void UCAbility_GroundBlast::HandleComboEvent(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UCAbility_GroundBlast::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (K2_HasAuthority())
    {
        BP_ApplyGameplayEffectToTarget(TargetData, DamageEffectDef.DamageEffect, GetAbilityLevel());
        PushTargets(TargetData, DamageEffectDef.PushVelocity);
    }

    FGameplayCueParameters CueParams;
    CueParams.Location     = UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetData, 1).ImpactPoint;
    CueParams.RawMagnitude = TargetAreaRadius;

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        ASC->ExecuteGameplayCue(GroundBlastCueTag, CueParams);
        if (AdditionalCueTag.IsValid())
        {
            ASC->ExecuteGameplayCue(AdditionalCueTag, CueParams);
        }
    }

    UAnimInstance* OwnerAnimInst = GetOwnerAnimInstance();
    if (OwnerAnimInst && CastMontage)
    {
        OwnerAnimInst->Montage_Play(CastMontage);
    }

    K2_EndAbility();
}

void UCAbility_GroundBlast::TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetData)
{
    K2_EndAbility();
}
