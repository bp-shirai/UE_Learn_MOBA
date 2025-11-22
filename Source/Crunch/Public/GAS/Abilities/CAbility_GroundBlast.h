// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_GroundBlast.generated.h"

class ACTargetActor_GroundPick;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_GroundBlast : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_GroundBlast();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TSubclassOf<ACTargetActor_GroundPick> TargetActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* GroundBlastMontage;

    UFUNCTION()
    void HandlePlayEnd(FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION()
    void HandleComboEvent(FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION()
    void TargetConfirmed(const FGameplayAbilityTargetDataHandle& TargetData);

    UFUNCTION()
    void TargetCanceled(const FGameplayAbilityTargetDataHandle& TargetData);
};
