// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_UpperCut.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Animation/AnimInstance.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"

#include "GAS/CGameplayTags.h"
#include "Math/MathFwd.h"

UCAbility_UpperCut::UCAbility_UpperCut()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;

    // SetAssetTags(FGameplayTagContainer(Tags::Ability::AbilityOne));

    BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
}

void UCAbility_UpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (K2_CommitAbility() == false)
    {
        K2_EndAbility();
        return;
    }

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, UpperCutMontage);
        PlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayMontage->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitLaunchEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::UpperCut_Launch);
        WaitLaunchEvent->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
        WaitLaunchEvent->ReadyForActivation();
    }

    NextComboName = NAME_None;
}

void UCAbility_UpperCut::StartLaunching(FGameplayEventData Data)
{
    if (K2_HasAuthority())
    {
        TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Data.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, true, ShouldDrawDebug());
        const FVector PushVelocity    = FVector::UpVector * UpperCutLaunchSpeed;

        PushTarget(GetAvatarActorFromActorInfo(), PushVelocity);

        for (const FHitResult& HitResult : HitResults)
        {
            PushTarget(HitResult.GetActor(), PushVelocity);

            ApplyGameplayEffectToHitResultActor(HitResult, LaunchDamageEffect, GetAbilityLevel(CurrentSpecHandle, CurrentActorInfo));
        }
    }

    UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo_Change, nullptr, false, false);
    WaitComboChangeEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboChangeEvent);
    WaitComboChangeEvent->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitComboCommitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::BasicAttack_Pressed);
    WaitComboCommitEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboCommitEvent);
    WaitComboCommitEvent->ReadyForActivation();

    UAbilityTask_WaitGameplayEvent* WaitComboDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Combo_Damage);
    WaitComboDamageEvent->EventReceived.AddDynamic(this, &ThisClass::HandleComboDamageEvent);
    WaitComboDamageEvent->ReadyForActivation();
}

void UCAbility_UpperCut::HandleComboChangeEvent(FGameplayEventData Data)
{
    FGameplayTag EventTag = Data.EventTag;

    if (EventTag == Tags::Ability::Combo_Change_End)
    {
        NextComboName = NAME_None;
        UE_LOG(LogTemp, Warning, TEXT("Next Combo is cleared"));
    }
    else
    {
        NextComboName = EventTag.GetTagLeafName();
        UE_LOG(LogTemp, Warning, TEXT("Next Combo is: %s"), *NextComboName.ToString());
    }
}

void UCAbility_UpperCut::HandleComboCommitEvent(FGameplayEventData Data)
{
    if (NextComboName == NAME_None) return;

    if (UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance())
    {
        UE_LOG(LogTemp, Warning, TEXT("Next Combo Commit is: %s"), *NextComboName.ToString());
        OwnerAnimInstance->Montage_SetNextSection(OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage), NextComboName, UpperCutMontage);
    }
}

void UCAbility_UpperCut::HandleComboDamageEvent(FGameplayEventData Data)
{
    if (K2_HasAuthority())
    {
        TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Data.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, true, ShouldDrawDebug());

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
    UAnimInstance* OwnerAnimInstance = GetOwnerAnimInstance();
    if (OwnerAnimInstance)
    {
        FName CurrentComboName                    = OwnerAnimInstance->Montage_GetCurrentSection(UpperCutMontage);
        const FCGenericDamageEffectDef* EffectDef = ComboDamageMap.Find(CurrentComboName);
        return EffectDef;
    }

    return nullptr;
}
