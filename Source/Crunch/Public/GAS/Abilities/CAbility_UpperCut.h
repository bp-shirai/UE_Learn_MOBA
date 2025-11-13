// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GAS/CGameplayAbilityTypes.h"
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
    UPROPERTY(EditDefaultsOnly, Category = "Combo")
    TMap<FName, FCGenericDamageEffectDef> ComboDamageMap;


    UPROPERTY(EditDefaultsOnly, Category = "Launch")
    TSubclassOf<UGameplayEffect> LaunchDamageEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Launch")
    float UpperCutLaunchSpeed{1000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Launch")
    float UpperComboHoldSpeed{100.f};

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* UpperCutMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetSweepSphereRadius{80.f};

    const FCGenericDamageEffectDef* GetDamageEffectDefForCurrentCombo() const;


    UFUNCTION()
    void StartLaunching(FGameplayEventData Data);

    UFUNCTION()
    void HandleComboChangeEvent(FGameplayEventData Data);

    UFUNCTION()
    void HandleComboCommitEvent(FGameplayEventData Data);

    UFUNCTION()
    void HandleComboDamageEvent(FGameplayEventData Data);

    FName NextComboName;
};
