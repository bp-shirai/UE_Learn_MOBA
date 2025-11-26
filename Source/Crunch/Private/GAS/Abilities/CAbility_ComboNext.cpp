// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_ComboNext.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "Animation/AnimMontage.h"
#include "Engine/HitResult.h"
#include "GameplayEffectTypes.h"

#include "GAS/CGameplayTags.h"
#include "GAS/AbilityTasks/CAbilityTask_PlayMontageWaitEvent.h"

UCAbility_ComboNext::UCAbility_ComboNext()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(FGameplayTagContainer(Tags::Ability::BasicAttack));
    // BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
}

void UCAbility_ComboNext::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    ComboCount         = 0;
    bComboInputAllowed = false;

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        const FName ComboSection    = ComboSectionNames.IsValidIndex(ComboCount) ? ComboSectionNames[ComboCount] : NAME_None;
        const FName ComboEndSection = ComboEndSectionNames.IsValidIndex(ComboCount) ? ComboEndSectionNames[ComboCount] : NAME_None;

        const FGameplayTagContainer Tags(Tags::Ability::Combo_Change);
        UCAbilityTask_PlayMontageWaitEvent* PlayMontage = UCAbilityTask_PlayMontageWaitEvent::CreatePlayMontageAndWaitProxyTags(this, NAME_None, ComboMontage, Tags, 1.f, ComboSection);
        // PlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::HandlePlayBlendOut);
        PlayMontage->OnCancelled.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnCompleted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnEvent.AddDynamic(this, &ThisClass::HandleComboEvent);
        PlayMontage->ReadyForActivation();

        if (ComboEndSection != NAME_None)
        {
            MontageSetNextSectionName(ComboSection, ComboEndSection);
        }
    }

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo_Damage);
        WaitTargetingEventTask->EventReceived.AddDynamic(this, &ThisClass::ComboDamageEventReceived);
        WaitTargetingEventTask->ReadyForActivation();
    }
}

TSubclassOf<UGameplayEffect> UCAbility_ComboNext::GetDamageEffectForCurrentCombo() const
{
    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        const FName CurrentComboName                       = ASC->GetCurrentMontageSectionName();
        const TSubclassOf<UGameplayEffect>* FoundEffectPtr = DamageEffectMap.Find(CurrentComboName);
        if (FoundEffectPtr)
        {
            return *FoundEffectPtr;
        }
    }

    return DefaultDamageEffect;
}

void UCAbility_ComboNext::SetupWaitComboInputPress()
{
    bComboInputAllowed = true;

    UAbilityTask_WaitInputPress* WaitInput = UAbilityTask_WaitInputPress::WaitInputPress(this);
    WaitInput->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
    WaitInput->ReadyForActivation();
}

void UCAbility_ComboNext::HandleInputPress(float TimeWaited)
{
    if (!bComboInputAllowed) return;

    ComboCount++;
    bComboInputAllowed = false;

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        const FName NextSection    = ComboSectionNames.IsValidIndex(ComboCount) ? ComboSectionNames[ComboCount] : NAME_None;
        const FName NextEndSection = ComboEndSectionNames.IsValidIndex(ComboCount) ? ComboEndSectionNames[ComboCount] : NAME_None;

        const FName CurrentComboName = ASC->GetCurrentMontageSectionName();

        MontageSetNextSectionName(CurrentComboName, NextSection);

        if (NextEndSection != NAME_None)
        {
            MontageSetNextSectionName(NextSection, NextEndSection);
        }
    }
}

void UCAbility_ComboNext::TryCommitCombo()
{
}

void UCAbility_ComboNext::DoDamage(FGameplayEventData Data)
{
}

void UCAbility_ComboNext::HandleComboEvent(FGameplayTag EventTag, FGameplayEventData EventData)
{
    if (EventTag == Tags::Ability::Combo_Change_Begin)
    {
        SetupWaitComboInputPress();
    }
    else if (EventTag == Tags::Ability::Combo_Change_End)
    {
        // Disable combo input
        bComboInputAllowed = false;
    }
}

void UCAbility_ComboNext::HandlePlayBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UCAbility_ComboNext::HandlePlayEnd(FGameplayTag EventTag, FGameplayEventData EventData)
{
    K2_EndAbility();
}

void UCAbility_ComboNext::HandleComboDamage(const FGameplayEventData& Data)
{
}

void UCAbility_ComboNext::ComboDamageEventReceived(FGameplayEventData Data)
{
    TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Data.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, true, ShouldDrawDebug());

    for (const FHitResult& HitResult : HitResults)
    {
        const TSubclassOf<UGameplayEffect> DamageEffect = GetDamageEffectForCurrentCombo();

        // Apply Damage Effect
        ApplyGameplayEffectToHitResultActor(HitResult, DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
    }
}
