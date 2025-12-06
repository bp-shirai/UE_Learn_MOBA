// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/CAbilitySystemStatics.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GAS/CAbilitySystemComponent.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffectTypes.h"
#include "GenericTeamAgentInterface.h"

#include "GAS/CGameplayTags.h"
#include "Serialization/JsonTypes.h"

int32 UCAbilitySystemStatics::GetGameplayTagID(const FGameplayTag& Tag)
{
    if (!Tag.IsValid()) return -1;
    return Tag.GetTagName().GetDisplayIndex().ToUnstableInt();
}

void UCAbilitySystemStatics::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
    ensureMsgf(ASC, TEXT("Actor %s does not have an AbilitySystemComponent"), *GetNameSafe(InActor));

    if (ASC && ASC->HasMatchingGameplayTag(TagToAdd) == false)
    {
        ASC->AddLooseGameplayTag(TagToAdd);
    }
}

void UCAbilitySystemStatics::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
    ensureMsgf(ASC, TEXT("Actor %s does not have an AbilitySystemComponent"), *GetNameSafe(InActor));

    if (ASC && ASC->HasMatchingGameplayTag(TagToRemove))
    {
        ASC->RemoveLooseGameplayTag(TagToRemove);
    }
}

bool UCAbilitySystemStatics::DoesActorHaveTag(const AActor* InActor, FGameplayTag TagToCheck)
{
    const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(InActor);
    ensureMsgf(ASC, TEXT("Actor %s does not have an AbilitySystemComponent"), *GetNameSafe(InActor));

    return ASC ? ASC->HasMatchingGameplayTag(TagToCheck) : false;
}

void UCAbilitySystemStatics::BP_DoesActorHaveTag(const AActor* InActor, FGameplayTag TagToCheck, ECConfirmType& OutConfirmType)
{
    if (!ensure(InActor))
    {
        OutConfirmType = ECConfirmType::No;
        return;
    }
    OutConfirmType = DoesActorHaveTag(InActor, TagToCheck) ? ECConfirmType::Yes : ECConfirmType::No;
}

bool UCAbilitySystemStatics::IsTargetPawnHostile(const APawn* QueryPawn, const APawn* TargetPawn)
{
    if (!ensure(QueryPawn && TargetPawn)) return false;

    const IGenericTeamAgentInterface* QueryTramAgent = Cast<IGenericTeamAgentInterface>(QueryPawn);
    if (QueryTramAgent)
    {
        return QueryTramAgent->GetTeamAttitudeTowards(*TargetPawn) == ETeamAttitude::Hostile;
    }

    return false;
}

float UCAbilitySystemStatics::GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel)
{
    return InScalableFloat.GetValueAtLevel(InLevel);
}

float UCAbilitySystemStatics::GetStaticColldownDurationForAbility(const UGameplayAbility* Ability)
{
    const UGameplayEffect* CooldownEffect = Ability ? Ability->GetCooldownGameplayEffect() : nullptr;
    if (!CooldownEffect) return 0.f;

    float CooldownDuration = 0.f;

    CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);
    return CooldownDuration;
}

float UCAbilitySystemStatics::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
    const UGameplayEffect* CostEffect = Ability ? Ability->GetCostGameplayEffect() : nullptr;
    if (!CostEffect || CostEffect->Modifiers.Num() == 0) return 0.f;

    float Cost = 0.f;
    CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);

    return FMath::Abs(Cost);
}

bool UCAbilitySystemStatics::IsHero(const AActor* ActorToCheck)
{
    const IAbilitySystemInterface* ActorASI = Cast<IAbilitySystemInterface>(ActorToCheck);
    const UAbilitySystemComponent* ActorASC = ActorASI ? ActorASI->GetAbilitySystemComponent() : nullptr;

    return ActorASC ? ActorASC->HasMatchingGameplayTag(Tags::Role::Hero) : false;
}

bool UCAbilitySystemStatics::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec)
{
    return Spec.Level >= 4;
}

bool UCAbilitySystemStatics::CheckAbilityCost(const FGameplayAbilitySpec& Spec, const UAbilitySystemComponent* ASC)
{
    const UGameplayAbility* AbilityCDO = Spec.Ability;
    if (!AbilityCDO || !ASC) return false;

    return AbilityCDO->CheckCost(Spec.Handle, ASC->AbilityActorInfo.Get());
}

float UCAbilitySystemStatics::GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent* ASC, int32 AbilityLevel)
{
    float ManaCost = 0.f;
    if (!AbilityCDO || !ASC) return ManaCost;

    const UGameplayEffect* CostEffect = AbilityCDO->GetCostGameplayEffect();
    if (CostEffect)
    {
        FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(CostEffect->GetClass(), AbilityLevel, ASC->MakeEffectContext());
        CostEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(*EffectSpec.Data.Get(), ManaCost);
    }

    return FMath::Abs(ManaCost);
}

float UCAbilitySystemStatics::GetCooldownDurationFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent* ASC, int32 AbilityLevel)
{
    float CooldownDuration = 0.f;
    if (AbilityCDO && ASC)
    {
        UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
        if (CooldownEffect)
        {
            FGameplayEffectSpecHandle EffectSpec = ASC->MakeOutgoingSpec(CooldownEffect->GetClass(), AbilityLevel, ASC->MakeEffectContext());
            CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(*EffectSpec.Data.Get(), CooldownDuration);
        }
    }

    return FMath::Abs(CooldownDuration);
}

float UCAbilitySystemStatics::GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent* ASC)
{
    if (!AbilityCDO || !ASC)
        return 0;

    UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
    if (!CooldownEffect)
        return 0;

    FGameplayEffectQuery CooldownEffectQuery;
    CooldownEffectQuery.EffectDefinition = CooldownEffect->GetClass();

    float CooldownRemaining                            = 0.f;
    FJsonSerializableArrayFloat CooldownTimeRemainings = ASC->GetActiveEffectsTimeRemaining(CooldownEffectQuery);

    for (const float Remaining : CooldownTimeRemainings)
    {
        if (Remaining > CooldownRemaining)
        {
            CooldownRemaining = Remaining;
        }
    }

    return CooldownRemaining;
}

bool UCAbilitySystemStatics::CheckAbilityCostStatic(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent* ASC)
{
    if (!AbilityCDO || !ASC) return false;

    return AbilityCDO->CheckCost(FGameplayAbilitySpecHandle(), ASC->AbilityActorInfo.Get());
}
