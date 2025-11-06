// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Combo.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystemComponent.h"

#include "GameplayTagsManager.h"

#include "GAS/CGameplayTags.h"

UCAbility_Combo::UCAbility_Combo()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    SetAssetTags(FGameplayTagContainer(Tags::Ability::BasicAttack));
    BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
}

void UCAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
        PlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo::_Change, nullptr, false, false);
        WaitEvent->EventReceived.AddDynamic(this, &ThisClass::ComboChangedEventReceived);
        WaitEvent->ReadyForActivation();
    }

    if (K2_HasAuthority())
    {
        UAbilityTask_WaitGameplayEvent* WaitTargetingEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo::Damage);
        WaitTargetingEventTask->EventReceived.AddDynamic(this, &ThisClass::ComboDamageEventReceived);
        WaitTargetingEventTask->ReadyForActivation();
    }

    SetupWaitComboInputPress();
}

void UCAbility_Combo::ComboChangedEventReceived(FGameplayEventData Data)
{
    FGameplayTag EventTag = Data.EventTag;

    if (EventTag == Tags::Ability::Combo::Change::End)
    {
        NextComboName = NAME_None;
        return;
    }
    else
    {
        NextComboName = EventTag.GetTagLeafName();
    }
}

void UCAbility_Combo::SetupWaitComboInputPress()
{
    UAbilityTask_WaitInputPress* WaitInput = UAbilityTask_WaitInputPress::WaitInputPress(this);
    WaitInput->OnPress.AddDynamic(this, &ThisClass::HandleInputPress);
    WaitInput->ReadyForActivation();
}

void UCAbility_Combo::HandleInputPress(float TimeWaited)
{
    SetupWaitComboInputPress();
    TryCommitCombo();
}

void UCAbility_Combo::TryCommitCombo()
{
    if (NextComboName == NAME_None) return;

    // if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
    // {
    //     OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(), NextComboName, ComboMontage);
    // }

    if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
    {
        const FName CurrentSectionName = ASC->GetCurrentMontageSectionName();
        MontageSetNextSectionName(CurrentSectionName, NextComboName);
    }
}

void UCAbility_Combo::ComboDamageEventReceived(FGameplayEventData Data)
{
    TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Data.TargetData, 30.f, true, true);


}
