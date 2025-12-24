// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Dash.h"

#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

#include "Engine/EngineTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/TimerHandle.h"
#include "GAS/CGameplayTags.h"
#include "GAS/TargetActor/CTargetActor_Around.h"

void UCAbility_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility() || !AM_Dash)
    {
        K2_EndAbility();
        return;
    }

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayDashMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_Dash);
        PlayDashMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayDashMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayDashMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayDashMontage->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitDashStartEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Dash_Start);
        WaitDashStartEvent->EventReceived.AddDynamic(this, &ThisClass::StartDash);
        WaitDashStartEvent->ReadyForActivation();
    }
}

void UCAbility_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo();
    if (OwnerASC && DashEffect.IsValid())
    {
        OwnerASC->RemoveActiveGameplayEffect(DashEffect);
    }

    if (PushForwardInputTimer.IsValid())
    {
        GetWorld()->GetTimerManager().ClearTimer(PushForwardInputTimer);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCAbility_Dash::StartDash(FGameplayEventData EventData)
{
    if (K2_HasAuthority())
    {
        DashEffect = BP_ApplyGameplayEffectToOwner(GE_Dash, GetAbilityLevel());
    }

    if (IsLocallyControlled())
    {
        PushForwardInputTimer           = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PushForward);
        OwnerCharacterMovementComponent = GetAvatarActorFromActorInfo()->GetComponentByClass<UCharacterMovementComponent>(); // GetOwningAvatarCharacter()->GetCharacterMovement();
    }

    UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, TargetActorClass);
    WaitTargetData->ValidData.AddDynamic(this, &ThisClass::TargetReceived);
    WaitTargetData->ReadyForActivation();

    AGameplayAbilityTargetActor* TargetActor;
    WaitTargetData->BeginSpawningActor(this, TargetActorClass, TargetActor);

    ACTargetActor_Around* TargetAround = CastChecked<ACTargetActor_Around>(TargetActor);
    TargetAround->ConfigureDetection(TargetDetectionRadius, GetOwnerTeamId(), LocalGameplayCueTag);

    WaitTargetData->FinishSpawningActor(this, TargetActor);

    TargetAround->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetActorAttachSocketName);
}

void UCAbility_Dash::PushForward()
{
    if (OwnerCharacterMovementComponent)
    {
        const FVector ForwardActor = GetAvatarActorFromActorInfo()->GetActorForwardVector();
        OwnerCharacterMovementComponent->AddInputVector(ForwardActor);
        PushForwardInputTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PushForward);
    }
}

void UCAbility_Dash::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetData)
{
    if (K2_HasAuthority())
    {
        BP_ApplyGameplayEffectToTarget(TargetData, GE_Damage, GetAbilityLevel());
        PushTargetsFromOwnerLocation(TargetData, TargetHitPushSpeed);
    }
}
