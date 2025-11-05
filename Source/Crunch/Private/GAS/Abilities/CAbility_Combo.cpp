// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Combo.h"

#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"

#include "GameplayTagsManager.h"

#include "GAS/CGameplayTags.h"

UCAbility_Combo::UCAbility_Combo()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(FGameplayTagContainer(Tags::Ability::BasicAttack));
    BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
}

void UCAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }
    // Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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

        SetupWaitComboInputPress();
    }

   
}

void UCAbility_Combo::ComboChangedEventReceived(FGameplayEventData Data)
{
    FGameplayTag EventTag = Data.EventTag;

    if (EventTag == Tags::Ability::Combo::Change::End)
    {
        // if (Tags::Ability::Combo::Change::End.GetTag().GetTagLeafName() != NextComboName)
        // {
        //     UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
        //     if (!OwnerAnimInstance) return;
        //     OwnerAnimInstance->Montage_JumpToSection(NextComboName, ComboMontage);
        //     UE_LOG(LogTemp, Warning, TEXT("Montage_JumpToSection: %s"), *NextComboName.ToString());
        // }

        NextComboName = NAME_None;
        return;
    }

    NextComboName = EventTag.GetTagLeafName();
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
    
    UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
    if (!OwnerAnimInstance) return;
    OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(), NextComboName, ComboMontage);
}
