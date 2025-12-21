// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Blackhole.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

#include "GAS/TargetActor/CTargetActor_GroundPick.h"
#include "GAS/TargetActor/CTargetActor_Blackhole.h"

UCAbility_Blackhole::UCAbility_Blackhole()
{
}

void UCAbility_Blackhole::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        K2_EndAbility();
        return;
    }

    PlayCastBlackholeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_Targeting);
    // PlayMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
    PlayCastBlackholeMontageTask->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
    PlayCastBlackholeMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
    PlayCastBlackholeMontageTask->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
    PlayCastBlackholeMontageTask->ReadyForActivation();

    UAbilityTask_WaitTargetData* WaitPlacementTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, TargetActorClass);
    WaitPlacementTask->ValidData.AddDynamic(this, &ThisClass::PlaceBlackhole);
    WaitPlacementTask->Cancelled.AddDynamic(this, &ThisClass::PlacementCancelled);
    WaitPlacementTask->ReadyForActivation();

    AGameplayAbilityTargetActor* TargetActor;
    WaitPlacementTask->BeginSpawningActor(this, TargetActorClass, TargetActor);
    if (ACTargetActor_GroundPick* GroundPick = Cast<ACTargetActor_GroundPick>(TargetActor))
    {
        GroundPick->SetShouldDrawDebug(ShouldDrawDebug());
        GroundPick->SetTargetAreaRadius(TargetAreaRadius);
        GroundPick->SetTargetTraceRange(TargetTraceRange);
    }
    WaitPlacementTask->FinishSpawningActor(this, TargetActor);

    AddAimEffect();
}

void UCAbility_Blackhole::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    RemoveEffect();
    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCAbility_Blackhole::PlaceBlackhole(const FGameplayAbilityTargetDataHandle& TargetData)
{
    
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    RemoveEffect();

    if (PlayCastBlackholeMontageTask)
    {
        PlayCastBlackholeMontageTask->OnCancelled.RemoveAll(this);
        PlayCastBlackholeMontageTask->OnInterrupted.RemoveAll(this);
        PlayCastBlackholeMontageTask->OnCompleted.RemoveAll(this);
    }

    if (HasAuthorityOrPredictionKey(CurrentActorInfo, &CurrentActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayHoldBlackholeMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_HoldBlackhole);
        PlayHoldBlackholeMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayHoldBlackholeMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayHoldBlackholeMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayHoldBlackholeMontage->ReadyForActivation();
    }

    BlackholeTargetingTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::UserConfirmed, BlackholeTargetActorClass);
    BlackholeTargetingTask->ValidData.AddDynamic(this, &ThisClass::FinalTargetsReceived);
    BlackholeTargetingTask->Cancelled.AddDynamic(this, &ThisClass::FinalTargetsReceived);
    BlackholeTargetingTask->ReadyForActivation();

    AGameplayAbilityTargetActor* TargetActor;
    BlackholeTargetingTask->BeginSpawningActor(this, BlackholeTargetActorClass, TargetActor);
    ACTargetActor_Blackhole* Blackhole = Cast<ACTargetActor_Blackhole>(TargetActor);
    if (Blackhole)
        Blackhole->ConfigureBlackhole(TargetAreaRadius, BlackholePullSpeed, BlackholeDuration, GetOwnerTeamId());

    BlackholeTargetingTask->FinishSpawningActor(this, TargetActor);

    if (Blackhole)
        Blackhole->SetActorLocation(UAbilitySystemBlueprintLibrary::GetHitResultFromTargetData(TargetData, 1).ImpactPoint);

    UE_LOG(LogTemp, Warning, TEXT("PlaceBlackhole"));

}

void UCAbility_Blackhole::PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetData)
{
    K2_EndAbility();
}

void UCAbility_Blackhole::AddAimEffect()
{
    AimHandle = BP_ApplyGameplayEffectToOwner(GE_Aim);
}

void UCAbility_Blackhole::RemoveEffect()
{
    if (AimHandle.IsValid())
    {
        BP_RemoveGameplayEffectFromOwnerWithHandle(AimHandle);
    }
}

void UCAbility_Blackhole::FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetData)
{
}
