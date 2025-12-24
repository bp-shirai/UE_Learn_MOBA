// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Animation/AnimMontage.h"
#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Freeze.generated.h"

class ACTargetActor_GroundPick;
/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Freeze : public UCGameplayAbility
{
    GENERATED_BODY()
public:
    UCAbility_Freeze();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetingRadius{1000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetingRange{2000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TSubclassOf<ACTargetActor_GroundPick> TargetActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AM_FreezeTargeting;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AM_FreezeCast;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> GE_Damage;

    UPROPERTY(EditDefaultsOnly, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
    FGameplayTag FreezingTargetCueTag;

    UPROPERTY(EditDefaultsOnly, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
    FGameplayTag FreezingCueTag;

    UFUNCTION()
    void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetData);

    UFUNCTION()
    void TargetingCancelled(const FGameplayAbilityTargetDataHandle& TargetData);
};
