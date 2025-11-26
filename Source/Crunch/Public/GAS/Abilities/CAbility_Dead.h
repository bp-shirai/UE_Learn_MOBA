// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Dead.generated.h"

/**
 *
 */
UCLASS()
class CRUNCH_API UCAbility_Dead : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_Dead();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    float RewardRange{1000.f};

    TArray<AActor*> GetRewardTargets() const;
};
