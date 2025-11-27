// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/MMC/MMC_LevelBased.h"

#include "GAS/CAttributeSet.h"
#include "GAS/CHeroAttributeSet.h"
#include "AbilitySystemComponent.h"

UMMC_LevelBased::UMMC_LevelBased()
{
    LevelCaptureDef.AttributeToCapture = UCHeroAttributeSet::GetLevelAttribute();
    LevelCaptureDef.AttributeSource    = EGameplayEffectAttributeCaptureSource::Target;

    RelevantAttributesToCapture.Add(LevelCaptureDef);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    UAbilitySystemComponent* ASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
    if (!ASC) return 0.f;

    float Level = 0.f;
    FAggregatorEvaluateParameters EvalParams;
    EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
    GetCapturedAttributeMagnitude(LevelCaptureDef, Spec, EvalParams, Level);

    float RateAttributeValue = ASC->GetNumericAttribute(RateAttribute);

    return (Level - 1) * RateAttributeValue;
}
