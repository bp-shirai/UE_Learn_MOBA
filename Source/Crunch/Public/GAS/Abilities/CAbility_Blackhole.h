// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Blackhole.generated.h"

class ACTargetActor_GroundPick;
class ACTargetActor_Blackhole;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Blackhole : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_Blackhole();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TSubclassOf<ACTargetActor_GroundPick> TargetActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TSubclassOf<ACTargetActor_Blackhole> BlackholeTargetActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetAreaRadius{1000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float BlackholePullSpeed{3000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetTraceRange{2000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float BlackholeDuration{6.f};

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AM_Targeting;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AM_HoldBlackhole;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TSubclassOf<UGameplayEffect> GE_Aim;

    FActiveGameplayEffectHandle AimHandle;

    UFUNCTION()
    void PlaceBlackhole(const FGameplayAbilityTargetDataHandle& TargetData);

    UFUNCTION()
    void PlacementCancelled(const FGameplayAbilityTargetDataHandle& TargetData);

    void AddAimEffect();
    void RemoveEffect();

    UPROPERTY()
    class UAbilityTask_PlayMontageAndWait* PlayCastBlackholeMontageTask;

    UPROPERTY()
    class UAbilityTask_WaitTargetData* BlackholeTargetingTask;

    UFUNCTION()
    void FinalTargetsReceived(const FGameplayAbilityTargetDataHandle& TargetData);
};
