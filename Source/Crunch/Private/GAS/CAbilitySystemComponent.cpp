// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CAbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Engine/DataTable.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "GAS/CGameplayAbility.h"
#include "GAS/CAttributeSet.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GAS/CHeroAttributeSet.h"
#include "GAS/CGameplayTags.h"
#include "GAS/Data/CAbilitySystemGenerics.h"

UCAbilitySystemComponent::UCAbilitySystemComponent()
{
    GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HealthUpdated);
    GetGameplayAttributeValueChangeDelegate(UCAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::ManaUpdated);

    GenericConfirmInputID = static_cast<int32>(ECAbilityInputID::Confirm);
    GenericCancelInputID  = static_cast<int32>(ECAbilityInputID::Cancel);
}

void UCAbilitySystemComponent::InitializeBaseAttributes()
{
    if (!GetOwner()) return;

    const UDataTable* BaseStatsDataTable = AbilitySystemGenerics ? AbilitySystemGenerics->GetBaseStatsDataTable() : nullptr;
    if (BaseStatsDataTable == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilitySystemComponent::InitializeBaseAttributes: StatsDataTable is nullptr : AvatarActor = %s"), *GetNameSafe(GetAvatarActor()));
        return;
    }

    const FCHeroBaseStats* BaseStats = nullptr;
    for (const auto& [RowName, DataRow] : BaseStatsDataTable->GetRowMap())
    {
        BaseStats = BaseStatsDataTable->FindRow<FCHeroBaseStats>(RowName, "");
        if (BaseStats && BaseStats->Class == GetOwner()->GetClass())
        {
            break;
        }
    }

    if (BaseStats)
    {
        SetNumericAttributeBase(UCAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
        SetNumericAttributeBase(UCAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
        SetNumericAttributeBase(UCAttributeSet::GetAttackAttribute(), BaseStats->BaseAttack);
        SetNumericAttributeBase(UCAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
        SetNumericAttributeBase(UCAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);

        SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
        SetNumericAttributeBase(UCHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowRate);
        SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
        SetNumericAttributeBase(UCHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowRate);
    }
}

void UCAbilitySystemComponent::ServerSideInit()
{
    InitializeBaseAttributes();
    ApplyInitialEffects();
    GiveInitialAbilities();
}

void UCAbilitySystemComponent::ApplyInitialEffects()
{
    if (!HasAuthority()) return;
    if (!AbilitySystemGenerics) return;

    for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySystemGenerics->GetInitialEffects())
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
        GiveAbility(FGameplayAbilitySpec(Ability, 1, static_cast<int32>(InputID), nullptr));
    }

    if (!AbilitySystemGenerics) return;
    for (const TSubclassOf<UGameplayAbility>& PassiveAbility : AbilitySystemGenerics->GetPassiveAbilities())
    {
        GiveAbility(FGameplayAbilitySpec(PassiveAbility, 1, -1, nullptr));
    }
}

void UCAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
    if (!GetOwner() || !GetOwner()->HasAuthority()) return;

    bool bFound           = false;
    const float MaxHealth = GetGameplayAttributeValue(UCAttributeSet::GetMaxHealthAttribute(), bFound);
    if (bFound && ChangeData.NewValue >= MaxHealth)
    {
        AddGameplayTagIfNone(Tags::Stats::Health_Full); // This is done local only
    }
    else
    {
        RemoveGameplayTagIfFound(Tags::Stats::Health_Full);
    }

    if (ChangeData.NewValue <= 0)
    {
        if (AddGameplayTagIfNone(Tags::Stats::Health_Empty))
        {
            if (AbilitySystemGenerics && AbilitySystemGenerics->GetDeathEffect())
            {
                AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());
            }

            // Send Dead Event Data
            FGameplayEventData DeadEventData;
            if (ChangeData.GEModData)
            {
                DeadEventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();
            }
            UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), Tags::Stats::Dead, DeadEventData);
        }
    }
    else
    {
        RemoveGameplayTagIfFound(Tags::Stats::Health_Empty);
    }
}

void UCAbilitySystemComponent::ManaUpdated(const FOnAttributeChangeData& Data)
{
    if (!GetOwner() || !GetOwner()->HasAuthority()) return;

    bool bFound         = false;
    const float MaxMana = GetGameplayAttributeValue(UCAttributeSet::GetMaxManaAttribute(), bFound);
    if (bFound && Data.NewValue >= MaxMana)
    {
        AddGameplayTagIfNone(Tags::Stats::Mana_Full); // This is done local only
    }
    else
    {
        RemoveGameplayTagIfFound(Tags::Stats::Mana_Full);
    }

    if (Data.NewValue <= 0)
    {
        AddGameplayTagIfNone(Tags::Stats::Mana_Empty);
    }
    else
    {
        RemoveGameplayTagIfFound(Tags::Stats::Mana_Empty);
    }
}

void UCAbilitySystemComponent::ApplyFullStatEffect()
{
    if (AbilitySystemGenerics)
    {
        AuthApplyGameplayEffect(AbilitySystemGenerics->GetFullStatEffect());
    }
}

void UCAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> ApplyEffect, int32 Level)
{
    if (HasAuthority() && ApplyEffect)
    {
        FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(ApplyEffect, Level, MakeEffectContext());
        BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
    }
}

bool UCAbilitySystemComponent::AddGameplayTagIfNone(const FGameplayTag& Tag)
{
    if (HasMatchingGameplayTag(Tag))
    {
        return false;
    }
    else
    {
        AddLooseGameplayTag(Tag);
        return true;
    }
}

bool UCAbilitySystemComponent::RemoveGameplayTagIfFound(const FGameplayTag& Tag)
{
    if (HasMatchingGameplayTag(Tag))
    {
        RemoveLooseGameplayTag(Tag);
        return true;
    }
    else
    {
        return false;
    }
}
