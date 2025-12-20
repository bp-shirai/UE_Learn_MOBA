// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/Abilities/CAbility_Shoot.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "Components/SkeletalMeshComponent.h"
#include "CoreGlobals.h"
#include "Engine/World.h"
#include "GAS/AbilityTasks/CAbilityTask_PlayMontageWaitEvent.h"
#include "GAS/CProjectileActor.h"
#include "GAS/CAbilitySystemStatics.h"
#include "GAS/CGameplayTags.h"
#include "GameplayEffect.h"
#include "GenericTeamAgentInterface.h"

UCAbility_Shoot::UCAbility_Shoot()
{
    ActivationOwnedTags.AddTag(Tags::Stats::Aim);
    ActivationOwnedTags.AddTag(Tags::Stats::Crosshair);
    ActivationOwnedTags.AddTag(Tags::Ability::BasicAttack_SendServer);
}

void UCAbility_Shoot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!K2_CommitAbility())
    {
        K2_EndAbility();
        return;
    }

    // UE_LOG(LogTemp, Warning, TEXT("Shoot Ability Activated"));

    if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
    {
        UAbilityTask_WaitGameplayEvent* WaitStartShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::BasicAttack_Pressed);
        WaitStartShootingEvent->EventReceived.AddDynamic(this, &ThisClass::StartShooting);
        WaitStartShootingEvent->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitStopShootingEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::BasicAttack_Released);
        WaitStopShootingEvent->EventReceived.AddDynamic(this, &ThisClass::StopShooting);
        WaitStopShootingEvent->ReadyForActivation();

        UAbilityTask_WaitGameplayEvent* WaitShootProjectileEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, Tags::Ability::Shoot, nullptr, false, false);
        WaitShootProjectileEvent->EventReceived.AddDynamic(this, &ThisClass::ShootProjectile);
        WaitShootProjectileEvent->ReadyForActivation();
    }
}

void UCAbility_Shoot::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    // UE_LOG(LogTemp, Warning, TEXT("Shoot Ability Ended"));
    K2_EndAbility();
}

void UCAbility_Shoot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (AimTargetASC)
    {
        AimTargetASC->RegisterGameplayTagEvent(Tags::Stats::Dead).RemoveAll(this);
        AimTargetASC = nullptr;
    }


    AimTarget = nullptr;

    SendLocalGameplayEvent(Tags::Ability::Target_Updated, FGameplayEventData());

    StopShooting(FGameplayEventData());

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCAbility_Shoot::StartShooting(FGameplayEventData Payload)
{
    UE_LOG(LogTemp, Warning, TEXT("Start Shooting"));
    if (K2_HasAuthority())
    {
        UAbilityTask_PlayMontageAndWait* PlayShootMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ShootMontage);
        PlayShootMontage->ReadyForActivation();
    }
    else
    {
        PlayMontageLocally(ShootMontage);
    }

    FindAimTarget();
    StartAimTargetCheckTimer();
}

void UCAbility_Shoot::StopShooting(FGameplayEventData Payload)
{
    UE_LOG(LogTemp, Warning, TEXT("Stop Shooting"));
    if (ShootMontage)
    {
        StopMontageAfterCurrentSection(ShootMontage);
    }

    StopAimTargetCheckTimer();
}

void UCAbility_Shoot::ShootProjectile(FGameplayEventData Payload)
{
    UE_LOG(LogTemp, Warning, TEXT("Shoot Projectile"));
    if (K2_HasAuthority())
    {
        AActor* OwnerAvatarActor = GetAvatarActorFromActorInfo();
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner                          = OwnerAvatarActor;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        FVector SocketLocation = OwnerAvatarActor->GetActorLocation();

        if (USkeletalMeshComponent* MeshComp = GetOwningComponentFromActorInfo())
        {
            FName SocketName = Payload.EventTag.GetTagLeafName();
            SocketLocation   = MeshComp->GetSocketLocation(SocketName);
        }

        ACProjectileActor* Projectile = GetWorld()->SpawnActor<ACProjectileActor>(ProjectileClass, SocketLocation, OwnerAvatarActor->GetActorRotation(), SpawnParams);
        if (Projectile)
        {
            // UE_LOG(LogTemp, Warning, TEXT("Shoot Projectile Success"));

            FGameplayEffectSpecHandle HitEffectHandle = MakeOutgoingGameplayEffectSpec(ProjectileHitEffect, GetAbilityLevel());
            Projectile->ShootProjectile(ShootProjectileSpeed, ShootProjectileRange, GetAimTargetIfValid(), GetOwnerTeamId(), HitEffectHandle);
        }
        else
        {
            // UE_LOG(LogTemp, Warning, TEXT("Shoot Projectile Failed"));
        }
    }
}

AActor* UCAbility_Shoot::GetAimTargetIfValid() const
{
    return HasValidTarget() ? AimTarget : nullptr;
}

void UCAbility_Shoot::FindAimTarget()
{
    if (HasValidTarget()) return;

    if (AimTargetASC)
    {
        AimTargetASC->RegisterGameplayTagEvent(Tags::Stats::Dead).RemoveAll(this);
        AimTargetASC = nullptr;
    }

    AimTarget = GetAimTarget(ShootProjectileRange, ETeamAttitude::Hostile);
    if (AimTarget)
    {
        AimTargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(AimTarget);
        if (AimTargetASC)
        {
            AimTargetASC->RegisterGameplayTagEvent(Tags::Stats::Dead).AddUObject(this, &ThisClass::TargetDeadTagUpdated);
        }
    }

    FGameplayEventData EventData;
    EventData.Target = AimTarget;
    SendLocalGameplayEvent(Tags::Ability::Target_Updated, EventData);
}

void UCAbility_Shoot::StartAimTargetCheckTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(AimTargetCheckTimer, this, &ThisClass::FindAimTarget, AimTargetCheckTimeInterval, true);
    }
}

void UCAbility_Shoot::StopAimTargetCheckTimer()
{
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(AimTargetCheckTimer);
    }
}

bool UCAbility_Shoot::HasValidTarget() const
{
    if (!AimTarget) return false;
    if (UCAbilitySystemStatics::IsActorDead(AimTarget)) return false;
    if (!IsTargetInRange()) return false;

    return true;
}

bool UCAbility_Shoot::IsTargetInRange() const
{
    if (!AimTarget) return false;

    float Distance = FVector::Distance(AimTarget->GetActorLocation(), GetAvatarActorFromActorInfo()->GetActorLocation());
    return (Distance <= ShootProjectileRange);
}

void UCAbility_Shoot::TargetDeadTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        FindAimTarget();
    }
}
