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

private:
    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Gameplay Effects")
    TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Gameplay Abilities")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

    UPROPERTY(EditDefaultsOnly, Category = "Crunch|Gameplay Abilities")
    TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>> BaseAbilities;

    FORCEINLINE bool HasAuthority() const { return GetOwner() && GetOwner()->HasAuthority(); }
};
