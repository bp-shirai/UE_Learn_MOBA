// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CAbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

#include "GAS/CGameplayAbility.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{

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

    for (const auto& Pair : Abilities)
    {   
        GiveAbility(FGameplayAbilitySpec(Pair.Value, 0, static_cast<int32>(Pair.Key), nullptr));
        //UE_LOG(LogTemp, Log, TEXT("UCAbilitySystemComponent::GiveInitialAbilities: %s"), *AbilityClass->GetName());
    }

    for (const auto& Pair : BaseAbilities)
    {   
        GiveAbility(FGameplayAbilitySpec(Pair.Value, 0, static_cast<int32>(Pair.Key), nullptr));
        //UE_LOG(LogTemp, Log, TEXT("UCAbilitySystemComponent::GiveInitialAbilities: %s"), *AbilityClass->GetName());
    }

}
