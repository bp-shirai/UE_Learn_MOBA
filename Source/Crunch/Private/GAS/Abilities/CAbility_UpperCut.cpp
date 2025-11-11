// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_UpperCut.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"

#include "GAS/CGameplayTags.h"

UCAbility_UpperCut::UCAbility_UpperCut()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(FGameplayTagContainer(Tags::Ability::AbilityOne));
    BlockAbilitiesWithTag.AddTag(Tags::Ability::AbilityOne);
}

void UCAbility_UpperCut::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (K2_CommitAbility())
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

        UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::UpperCut::Launch);
        WaitEvent->EventReceived.AddDynamic(this, &ThisClass::StartLaunching);
        WaitEvent->ReadyForActivation();
    }
}

void UCAbility_UpperCut::StartLaunching(FGameplayEventData Data)
{
    if (!K2_HasAuthority())return;

    TArray<FHitResult> HitResults = GetHitResultsFromSweepLocationTargetData(Data.TargetData, TargetSweepSphereRadius, ETeamAttitude::Hostile, ShouldDrawDebug());
    const FVector PushVelocity = FVector::UpVector * UpperCutLaunchSpeed;

    PushTarget(GetAvatarActorFromActorInfo(), PushVelocity);


    for (const FHitResult& HitResult : HitResults)
    {
        PushTarget(HitResult.GetActor(), PushVelocity);
    }
}
