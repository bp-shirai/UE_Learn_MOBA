// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CAbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{

}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
    if (!GetOwner() || !GetOwner()->HasAuthority()) return;

    for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
    {
        FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
        ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    }
}

