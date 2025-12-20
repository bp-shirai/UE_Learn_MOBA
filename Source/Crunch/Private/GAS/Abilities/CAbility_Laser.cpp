// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Laser.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

#include "GAS/CAttributeSet.h"
#include "GAS/CGameplayTags.h"
#include "GAS/TargetActor/CTargetActor_Line.h"

UCAbility_Laser::UCAbility_Laser()
{
}

void UCAbility_Laser::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility() || !AM_Laser)
    {
        K2_EndAbility();
        return;
    }

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        UAbilityTask_PlayMontageAndWait* PlayLaserMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, AM_Laser);
        PlayLaserMontage->OnBlendOut.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayLaserMontage->OnInterrupted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayLaserMontage->OnCancelled.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayLaserMontage->OnCompleted.AddDynamic(this, &ThisClass::K2_EndAbility);
        PlayLaserMontage->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitShootEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Laser_Shoot);
        WaitShootEvent->EventReceived.AddDynamic(this, &ThisClass::ShootLaser);
        WaitShootEvent->ReadyForActivation();

        UAbilityTask_WaitCancel* WaitCancelEvent = UAbilityTask_WaitCancel::WaitCancel(this);
        WaitCancelEvent->OnCancel.AddDynamic(this, &ThisClass::K2_EndAbility);
        WaitCancelEvent->ReadyForActivation();
    }
}

void UCAbility_Laser::ShootLaser(FGameplayEventData Data)
{
    if (K2_HasAuthority())
    {
        OnGoingConsumptionHandle = BP_ApplyGameplayEffectToOwner(GE_OnGoingConsumption, GetAbilityLevel());
    }

    if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
    {
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::OnManaChanged);
    }

    // we'll spawn here on the local client.
    // you get this really quick response to immediately on your client.
    UAbilityTask_WaitTargetData* WaitDamageTargetTask = UAbilityTask_WaitTargetData::WaitTargetData(this, NAME_None, EGameplayTargetingConfirmation::CustomMulti, LaserTargetActorClass);
    WaitDamageTargetTask->ValidData.AddDynamic(this, &ThisClass::TargetReceived);
    WaitDamageTargetTask->ReadyForActivation();

    AGameplayAbilityTargetActor* TargetActor;
    WaitDamageTargetTask->BeginSpawningActor(this, LaserTargetActorClass, TargetActor);
    ACTargetActor_Line* LineTargetActor = Cast<ACTargetActor_Line>(TargetActor);

    if (LineTargetActor)
        LineTargetActor->ConfigureTargetSetting(TargetRange, DetectionCylinderRadius, TargetingInterval, GetOwnerTeamId(), ShouldDrawDebug());

    WaitDamageTargetTask->FinishSpawningActor(this, TargetActor);

    if (LineTargetActor)
        LineTargetActor->AttachToComponent(GetOwningComponentFromActorInfo(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TargetActorAttachSocketName);
}

void UCAbility_Laser::OnManaChanged(const FOnAttributeChangeData& Data)
{
    if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
    {
        if (!OwnerASC->CanApplyAttributeModifiers(GE_OnGoingConsumption.GetDefaultObject(), GetAbilityLevel(), MakeEffectContext(CurrentSpecHandle, CurrentActorInfo)))
        {
            // K2_EndAbility();
            K2_EndAbilityLocally();
        }
    }
}

void UCAbility_Laser::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (UAbilitySystemComponent* OwnerASC = GetAbilitySystemComponentFromActorInfo())
    {
        if (OnGoingConsumptionHandle.IsValid())
        {
            OwnerASC->RemoveActiveGameplayEffect(OnGoingConsumptionHandle);
        }

        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).RemoveAll(this);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCAbility_Laser::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
    UE_LOG(LogTemp, Warning, TEXT("Laser Ability Cancel"));
    Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UCAbility_Laser::TargetReceived(const FGameplayAbilityTargetDataHandle& TargetData)
{
    if (K2_HasAuthority())
    {
        BP_ApplyGameplayEffectToTarget(TargetData, GE_HitDamage, GetAbilityLevel());
    }

    PushTargets(TargetData, GetAvatarActorFromActorInfo()->GetActorForwardVector() * HitPushSpeed);
}
