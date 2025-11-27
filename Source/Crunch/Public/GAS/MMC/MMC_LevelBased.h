// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_LevelBased.generated.h"

/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UMMC_LevelBased : public UGameplayModMagnitudeCalculation
{
    GENERATED_BODY()
	
public:
    UMMC_LevelBased();

    virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
    UPROPERTY(EditDefaultsOnly)
    FGameplayAttribute RateAttribute;

    FGameplayEffectAttributeCaptureDefinition LevelCaptureDef;

};
