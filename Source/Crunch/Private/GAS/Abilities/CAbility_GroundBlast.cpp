// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_GroundBlast.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

#include "GAS/AbilityTasks/CAbilityTask_PlayMontageWaitEvent.h"
#include "GAS/CGameplayTags.h"
#include "GAS/TargetActor/CTargetActor_GroundPick.h"
#include "UObject/UnrealNames.h"

UCAbility_GroundBlast::UCAbility_GroundBlast()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    SetAssetTags(FGameplayTagContainer(Tags::Ability::AbilityTwo));

    BlockAbilitiesWithTag.AddTag(Tags::Ability::BasicAttack);
    ActivationOwnedTags.AddTag(Tags::Stats::Aim);
}

void UCAbility_GroundBlast::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
    {
        FGameplayTagContainer Tags(Tags::Ability::Combo_Damage);

        UCAbilityTask_PlayMontageWaitEvent* PlayMontage = UCAbilityTask_PlayMontageWaitEvent::CreatePlayMontageAndWaitProxyTags(this, NAME_None, GroundBlastMontage, Tags);
        // PlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnCancelled.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnCompleted.AddDynamic(this, &ThisClass::HandlePlayEnd);
        PlayMontage->OnEvent.AddDynamic(this, &ThisClass::HandleComboEvent);
        PlayMontage->ReadyForActivation();

        UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
        WaitTargetData->ValidData.AddDynamic(this, &ThisClass::TargetConfirmed);
        WaitTargetData->Cancelled.AddDynamic(this, &ThisClass::TargetCanceled);
        WaitTargetData->ReadyForActivation();

        AGameplayAbilityTargetActor* TargetActor;
        WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);
        WaitTargetData->FinishSpawningActor(this, TargetActor);
    }
}

void UCAbility_GroundBlast::HandlePlayEnd(FGameplayTag EventTag, FGameplayEventData EventData)
{
    K2_EndAbility();
}

void UCAbility_GroundBlast::HandleComboEvent(FGameplayTag EventTag, FGameplayEventData EventData)
{
}

void UCAbility_GroundBlast::TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetData)
{
    TArray<AActor*> TargetActors = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetData);

    for (AActor* TargetActor : TargetActors)
    {
        UE_LOG(LogTemp, Warning, TEXT("Target Actor: %s"), *GetNameSafe(TargetActor));
    }

    if (TargetActors.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("UCAbility_GroundBlast::TargetConfirmed - No Target Actors"));
        K2_EndAbility();
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("UCAbility_GroundBlast::TargetConfirmed"));
    K2_EndAbility();
}

void UCAbility_GroundBlast::TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetData)
{
    UE_LOG(LogTemp, Warning, TEXT("UCAbility_GroundBlast::TargetCanceled"));
    K2_EndAbility();
}
