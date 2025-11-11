// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_UpperCut.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_UpperCut : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_UpperCut();
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Launch")
    float UpperCutLaunchSpeed{1000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* UpperCutMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetSweepSphereRadius{80.f};

    UFUNCTION()
    void StartLaunching(FGameplayEventData Data);
};
