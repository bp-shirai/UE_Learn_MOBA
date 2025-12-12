// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/AbilityToolTip.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAbilityToolTip::SetAbilityInfo(const FName& AbilityName, UTexture2D* AbilityTexture, const FText& AbilityDescription, float AbilityCooldown, float AbilityCost)
{
    AbilityNameText->SetText(FText::FromName(AbilityName));
    AbilityIcon->SetBrushFromTexture(AbilityTexture);

    AbilityDescriptionText->SetText(AbilityDescription);

    FNumberFormattingOptions FormattingOptions;
    FormattingOptions.MaximumFractionalDigits = 0;

    AbilityCooldownText->SetText(FText::AsNumber(AbilityCooldown, &FormattingOptions));
    AbilityCostText->SetText(FText::AsNumber(AbilityCost, &FormattingOptions));
}
