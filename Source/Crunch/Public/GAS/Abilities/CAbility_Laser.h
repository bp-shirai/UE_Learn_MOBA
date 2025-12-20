// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Laser.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Laser : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_Laser();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
    virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AM_Laser;

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    TSubclassOf<UGameplayEffect> GE_OnGoingConsumption;

    FActiveGameplayEffectHandle OnGoingConsumptionHandle;

    UFUNCTION()
    void ShootLaser(FGameplayEventData Data);

    void OnManaChanged(const FOnAttributeChangeData& Data);
};
