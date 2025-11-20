// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "CAbility_ComboNext.generated.h"

class UAbilityTask_WaitInputPress;
class UCAbilityTask_PlayMontageWaitEvent;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_ComboNext : public UCGameplayAbility
{
    GENERATED_BODY()
public:
    UCAbility_ComboNext();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* ComboMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TArray<FName> ComboSectionNames;

    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    TArray<FName> ComboEndSectionNames;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetSweepSphereRadius{30.f};

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TSubclassOf<UGameplayEffect> DefaultDamageEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TMap<FName, TSubclassOf<UGameplayEffect>> DamageEffectMap;

    TSubclassOf<UGameplayEffect> GetDamageEffectForCurrentCombo() const;

    UFUNCTION()
    void HandlePlayEnd(FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION()
    void HandlePlayBlendOut(FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION()
    void HandleComboEvent(FGameplayTag EventTag, FGameplayEventData EventData);

    UFUNCTION()
    void ComboDamageEventReceived(FGameplayEventData Data);

    void HandleComboDamage(const FGameplayEventData& Data);

    void SetupWaitComboInputPress();

    UFUNCTION()
    void HandleInputPress(float TimeWaited);

    void TryCommitCombo();

    UFUNCTION()
    void DoDamage(FGameplayEventData Data);

    bool bComboInputAllowed;

    int32 ComboCount;
};
