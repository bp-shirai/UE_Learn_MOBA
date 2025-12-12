// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/CGameplayAbility.h"
#include "CAbility_Shoot.generated.h"


class ACProjectileActor;
/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCAbility_Shoot : public UCGameplayAbility
{
    GENERATED_BODY()

public:
    UCAbility_Shoot();

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    TSubclassOf<UGameplayEffect> ProjectileHitEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    float ShootProjectileSpeed = 2000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    float ShootProjectileRange = 3000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Shoot")
    TSubclassOf<ACProjectileActor> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Anim")
    UAnimMontage* ShootMontage;

    UFUNCTION()
    void StartShooting(FGameplayEventData Payload);

    UFUNCTION()
    void StopShooting(FGameplayEventData Payload);

    UFUNCTION()
    void ShootProjectile(FGameplayEventData Payload);

    AActor* GetAimTargetIfValid() const;

    UPROPERTY()
    AActor* AimTarget;

    UPROPERTY()
    UAbilitySystemComponent* AimTargetASC;

    FTimerHandle AimTargetCheckTimer;

    void FindAimTarget();

    UPROPERTY(EditDefaultsOnly, Category = "Target")
    float AimTargetCheckTimeInterval = 0.1f;

    void StartAimTargetCheckTimer();
    void StopAimTargetCheckTimer();

    bool HasValidTarget() const;
    bool IsTargetInRange() const;

    void TargetDeadTagUpdated(const FGameplayTag Tag, int32 NewCount);
};
