// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_UpperCut.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "GenericTeamAgentInterface.h"

#include "GAS/CGameplayAbilityTypes.h"
#include "GAS/CGameplayTags.h"
#include "GAS/AbilityTasks/CAbilityTask_PlayMontageWaitEvent.h"

UCAbility_UpperCut::UCAbility_UpperCut()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(FGameplayTagContainer(Tags::Ability::AbilityOne));

    BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
}

void UCAbility_UpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (K2_CommitAbility() == false)
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

        FGameplayTagContainer Tags(Tags::Ability::Combo_Change);
        Tags.AddTag(Tags::Ability::UpperCut_Launch);
        Tags.AddTag(Tags::Ability::Combo_Damage);
        Tags.AddTag(Tags::Ability::BasicAttack_Pressed);

        UCAbilityTask_PlayMontageWaitEvent* PlayMontage = UCAbilityTask_PlayMontageWaitEvent::CreatePlayMontageAndWaitProxyTags(this, NAME_None, UpperCutMontage, Tags, 1.f, ComboSection);
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

    // if (K2_HasAuthority())
    // {
    //     UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo_Damage);
    //     WaitTargetingEventTask->EventReceived.AddDynamic(this, &ThisClass::HandleComboDamageEvent);
    //     WaitTargetingEventTask->ReadyForActivation();
    // }

    // if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    // {
    //     UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
    //     PlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
    //     PlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
    //     PlayMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
    //     PlayMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
    //     PlayMontage->ReadyForActivation();

    //     UAbilityTask_WaitGameplayEvent* WaitLaunchEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::UpperCut_Launch);
    //     WaitLaunchEvent->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
    //     WaitLaunchEvent->ReadyForActivation();
    // }

    // NextComboName = NAME_None;
}

void UCAbility_UpperCut::StartLaunching(FGameplayTag EventTag, const FGameplayEventData& EventData)
{
    if (K2_HasAuthority())
    // if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
    {
        TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, true, ShouldDrawDebug());
        const FVector PushVelocity    = FVector::UpVector * UpperCutLaunchSpeed;

        PushTarget(GetAvatarActorFromActorInfo(), PushVelocity);

        for (const FHitResult& HitResult : HitResults)
        {
            PushTarget(HitResult.GetActor(), PushVelocity);

            ApplyGameplayEffectToHitResultActor(HitResult, LaunchDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
        }
    }

    // UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo_Change, nullptr, false, false);
    // WaitComboChangeEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboChangeEvent);
    // WaitComboChangeEvent->ReadyForActivation();

    // UAbilityTask_WaitGameplayEvent* WaitComboCommitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::BasicAttack_Pressed);
    // WaitComboCommitEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboCommitEvent);
    // WaitComboCommitEvent->ReadyForActivation();

    // UAbilityTask_WaitGameplayEvent* WaitComboDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo_Damage);
    // WaitComboDamageEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboDamageEvent);
    // WaitComboDamageEvent->ReadyForActivation();
}

void UCAbility_UpperCut::HandleComboChangeEvent(FGameplayTag EventTag, const FGameplayEventData& EventData)
{
    if (EventTag == Tags::Ability::Combo_Change_Begin)
    {
        bComboInputAllowed = true;
    }
    else if (EventTag == Tags::Ability::Combo_Change_End)
    {
        bComboInputAllowed = false;
    }
}

void UCAbility_UpperCut::HandleComboCommitEvent(FGameplayTag EventTag, const FGameplayEventData& EventData)
{
    if (!bComboInputAllowed) return;

    ComboCount++;
    bComboInputAllowed = false;

    if (const UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
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
    // if (NextComboName == NAME_None) return;

    // if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("Next Combo Commit is: %s"), *NextComboName.ToString());
    //     OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage), NextComboName, UpperCutMontage);
    // }
}

void UCAbility_UpperCut::HandleComboDamageEvent(FGameplayTag EventTag, const FGameplayEventData& EventData)
{
    if (K2_HasAuthority())
    {
        TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(EventData.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, true, ShouldDrawDebug());

        const FCGenericDamageEffectDef* DamageDef = GetDamageEffectDefForCurrentCombo();
        if (!DamageDef)
        {
            return;
        }

        PushTarget(GetAvatarActorFromActorInfo(), FVector::UpVector * UpperComboHoldSpeed);

        for (const FHitResult& HitResult : HitResults)
        {
            FVector PushVelocity = GetAvatarActorFromActorInfo()->GetActorTransform().TransformVector(DamageDef->PushVelocity);
            PushTarget(HitResult.GetActor(), PushVelocity);
            ApplyGameplayEffectToHitResultActor(HitResult, DamageDef->DamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
        }
    }
}

const FCGenericDamageEffectDef* UCAbility_UpperCut::GetDamageEffectDefForCurrentCombo() const
{
    const UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
    if (OwnerAnimInstance)
    {
        const FName CurrentComboName              = OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage);
        const FCGenericDamageEffectDef* EffectDef = ComboDamageMap.Find(CurrentComboName);
        return EffectDef;
    }

    return nullptr;
}

void UCAbility_UpperCut::HandlePlayEnd(FGameplayTag EventTag, FGameplayEventData EventData)
{
    K2_EndAbility();
}

void UCAbility_UpperCut::HandlePlayBlendOut(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UCAbility_UpperCut::HandleComboEvent(FGameplayTag EventTag, FGameplayEventData EventData)
{
    if (EventTag.MatchesTag(Tags::Ability::Combo_Change))
    {
        HandleComboChangeEvent(EventTag, EventData);
    }
    else if (EventTag == Tags::Ability::UpperCut_Launch)
    {
        StartLaunching(EventTag, EventData);
    }
    else if (EventTag == Tags::Ability::Combo_Damage)
    {
        HandleComboDamageEvent(EventTag, EventData);
    }
    else if (EventTag == Tags::Ability::BasicAttack_Pressed)
    {
        HandleComboCommitEvent(EventTag, EventData);
    }
}
