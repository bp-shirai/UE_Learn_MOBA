// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Tornado.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Tornado : public UCGameplayAbility
{
    GENERATED_BODY()
public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> GE_HitDamage;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    float HitPushSpeed{3000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AM_Tornado;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    float TornadoDuration{4.f};

    UFUNCTION()
    void TornadoDamageEventReceived(FGameplayEventData EventData);
};
