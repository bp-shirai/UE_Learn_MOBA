// Fill out your copyright notice in the Description page of Project Settings.

#include "GAS/MMC/MMC_BaseAttackDamage.h"

#include "GAS/CAttributeSet.h"
#include "GameplayEffectAggregator.h"
#include "Logging/LogVerbosity.h"

UMMC_BaseAttackDamage::UMMC_BaseAttackDamage()
{
    AttackCaptureDef.AttributeToCapture = UCAttributeSet::GetAttackAttribute();
    AttackCaptureDef.AttributeSource    = EGameplayEffectAttributeCaptureSource::Source;

    ArmorCaptureDef.AttributeToCapture = UCAttributeSet::GetArmorAttribute();
    ArmorCaptureDef.AttributeSource    = EGameplayEffectAttributeCaptureSource::Target;

    RelevantAttributesToCapture.Add(AttackCaptureDef);
    RelevantAttributesToCapture.Add(ArmorCaptureDef);
}

float UMMC_BaseAttackDamage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
    FAggregatorEvaluateParameters EvalParams;
    EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    float Attack = 0.f;
    GetCapturedAttributeMagnitude(AttackCaptureDef, Spec, EvalParams, Attack);

    float Armor = 0.f;
    GetCapturedAttributeMagnitude(ArmorCaptureDef, Spec, EvalParams, Armor);

    float DamageReduction = Armor / (Armor + 100);

    // float Damage = Attack * (1 - Armor) / (Armor + 100);
    float Damage = Attack * (1 - DamageReduction);

    UE_LOG(LogTemp, Log, TEXT("Damage = %f"), Damage);

    return -Damage;
}
