// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Combo.h"

#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "GAS/CGameplayTags.h"

UCAbility_Combo::UCAbility_Combo()
{
    SetAssetTags(FGameplayTagContainer(Tags::Ability::BasicAttack));
    BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
}

void UCAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    UE_LOG(LogTemp, Warning, TEXT("UCAbility_Combo::ActivateAbility"));
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }
    // Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
        Task->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
        Task->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        Task->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        Task->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        Task->ReadyForActivation();
    }
}
