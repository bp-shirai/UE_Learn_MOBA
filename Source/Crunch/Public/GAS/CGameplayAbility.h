// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include "CGameplayAbility.generated.h"

class ACharacter;

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UCGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()
public:
    UCGameplayAbility();
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
    UAnimInstance* GetOwnerAnimInstance() const;

    TArray<FHitResult> GetHitResultsFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius = 30.f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile,
                                                                bool bIgnoreSelf = true, bool bDrawDebug = false);

    UFUNCTION()
    FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }

    void PushSelf(const FVector& PushVelocity);
    void PushTarget(AActor* Target, const FVector& PushVelocity);
    void PushTargets(const TArray<AActor*> Targets, const FVector& PushVelocity);
    void PushTargets(const FGameplayAbilityTargetDataHandle& TargetData, const FVector& PushVelocity);
	void PushTargetsFromOwnerLocation(const TArray<AActor*>& Targets, float PushSpeed);
	void PushTargetsFromOwnerLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float PushSpeed);
	void PushTargetsFromLocation(const FGameplayAbilityTargetDataHandle& TargetDataHandle, const FVector& FromLocation, float PushSpeed);
	void PushTargetsFromLocation(const TArray<AActor*>& Targets, const FVector& FromLocation, float PushSpeed);
    
    ACharacter* GetOwningAvatarCharacter();

    void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffectClass, int32 Level);
    void SendLocalGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
    AActor* GetAimTarget(float AimDistance, ETeamAttitude::Type TeamAttitude) const;
    FGenericTeamId GetOwnerTeamId() const;
    bool IsActorTeamAttitudeIs(const AActor* OtherActor, ETeamAttitude::Type TeamAttitude) const;
    void PlayMontageLocally(UAnimMontage* MontageToPlay);
    void StopMontageAfterCurrentSection(UAnimMontage* MontageToStop);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Debug")
    bool bShouldDrawDebug{false};

    UPROPERTY()
    TWeakObjectPtr<ACharacter> AvatarCharacter;
};
