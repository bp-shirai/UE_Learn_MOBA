// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "GAS/CGameplayAbilityTypes.h"
#include "CAbilitySystemComponent.generated.h"

class UGameplayAbility;
class UCAbilitySystemGenerics;
/**
 *
 */
UCLASS()
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UCAbilitySystemComponent();

    void InitializeBaseAttributes();
    void ServerSideInit();
    void ApplyFullStatEffect();
    // Get the Abilities that is unique for the avatar actor, this do not include Generic/Basic ones.
    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }
    bool IsAtMaxLevel() const;

private:
    void ApplyInitialEffects();
    void GiveInitialAbilities();
    void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyEffect, int32 Level = 1);

    void HealthUpdated(const FOnAttributeChangeData& Data);
    void ManaUpdated(const FOnAttributeChangeData& Data);
    void ExperienceUpdated(const FOnAttributeChangeData& Data);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BaseAbilities;

    FORCEINLINE bool HasAuthority() const { return IsOwnerActorAuthoritative(); }

    bool AddGameplayTagIfNone(const FGameplayTag& Tag);
    bool RemoveGameplayTagIfFound(const FGameplayTag& Tag);

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
    UCAbilitySystemGenerics* AbilitySystemGenerics;
};
