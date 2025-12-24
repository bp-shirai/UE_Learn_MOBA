// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Dash.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Dash : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Animation")
    UAnimMontage* AM_Dash;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    float TargetDetectionRadius{300.f};

    UPROPERTY(EditDefaultsOnly, Category = "GameplayCue", meta = (Categories = "GameplayCue"))
    FGameplayTag LocalGameplayCueTag;

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    FName TargetActorAttachSocketName{"TargetDashCenter"};

    UPROPERTY(EditDefaultsOnly, Category = "Targeting")
    TSubclassOf<class ACTargetActor_Around> TargetActorClass;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    float TargetHitPushSpeed{3000.f};

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> GE_Damage;

    UPROPERTY(EditDefaultsOnly, Category = "Effect")
    TSubclassOf<UGameplayEffect> GE_Dash;

    FActiveGameplayEffectHandle DashEffect;
    FTimerHandle PushForwardInputTimer;

    void PushForward();

    class UCharacterMovementComponent* OwnerCharacterMovementComponent;

    UFUNCTION()
    void StartDash(FGameplayEventData EventData);

    UFUNCTION()
    void TargetReceived(const FGameplayAbilityTargetDataHandle& TargetData);


};
