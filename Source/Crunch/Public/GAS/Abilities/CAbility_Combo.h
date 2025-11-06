// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "CAbility_Combo.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Combo : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_Combo();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Animation")
    UAnimMontage* ComboMontage;

    UFUNCTION()
    void ComboChangedEventReceived(FGameplayEventData Data);

    UFUNCTION()
    void ComboDamageEventReceived(FGameplayEventData Data);

    void SetupWaitComboInputPress();

    UFUNCTION()
    void HandleInputPress(float TimeWaited);

    void TryCommitCombo();

    FName NextComboName;

    FGameplayTag NextComboTag;
};
