// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CAbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

#include "GAS/CGameplayAbility.h"
#include "GAS/CAttributeSet.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
    GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HealthUpdated);
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
    if (!HasAuthority()) return;

    for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
    {
        FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
        ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    }
}

void UCAbilitySystemComponent::GiveInitialAbilities()
{
    if (!HasAuthority()) return;

    for (const auto& [InputID, Ability] : Abilities)
    {
        GiveAbility(FGameplayAbilitySpec(Ability, 0, static_cast<int32>(InputID), nullptr));
    }

    for (const auto& [InputID, Ability] : BaseAbilities)
    {
        GiveAbility(FGameplayAbilitySpec(Ability, 0, static_cast<int32>(InputID), nullptr));
    }
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& Data)
{
    if (Data.NewValue <= 0 )
    {
        AuthApplyGameplayEffect(DeathEffect);
    }
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
    AuthApplyGameplayEffect(FullStatEffect);
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyEffect, int32 Level)
{
    if (HasAuthority() && ApplyEffect)
    {
        FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(ApplyEffect, Level, MakeEffectContext());
        BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
    }
}
