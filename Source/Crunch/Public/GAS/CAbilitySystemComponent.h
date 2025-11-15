// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "GAS/CGameplayAbilityTypes.h"
#include "CAbilitySystemComponent.generated.h"

class UGameplayAbility;

/**
 *
 */
UCLASS()
class CRUNCH_API UCAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UCAbilitySystemComponent();

    void ApplyInitialEffects();
    void GiveInitialAbilities();
    void ApplyFullStatEffect();
    // Get the Abilities that is unique for the avatar actor, this do not include Generic/Basic ones.
    const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const { return Abilities; }


private:
    void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyEffect, int32 Level = 1);
    void HealthUpdated(const FOnAttributeChangeData& Data);

private:
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TSubclassOf<UGameplayEffect> FullStatEffect;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BaseAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
    TSubclassOf<UGameplayEffect> DeathEffect;

    FORCEINLINE bool HasAuthority() const { return IsOwnerActorAuthoritative(); }
};
