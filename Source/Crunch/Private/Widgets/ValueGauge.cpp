// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ValueGauge.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "GameplayEffectTypes.h"

#include "GAS/CAbilitySystemComponent.h"
#include "GAS/CAttributeSet.h"

void UValueGauge::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (ProgressBar)
    {
        ProgressBar->SetFillColorAndOpacity(BarColor);
    }
    if (ValueText)
    {
        ValueText->SetFont(ValueTextFont);
        ValueText->SetVisibility(bValueTextVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        ProgressBar->SetVisibility(bProgressBarVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
    CachedValue    = NewValue;
    CachedMaxValue = NewMaxValue;

    if (NewMaxValue == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Value Gauge: %s, NewMaxValue can't be 0 : %s"), *GetName(), *GetOwningPlayer()->GetName());
        return;
    }

    const float NewPercent = NewValue / NewMaxValue;

    ProgressBar->SetPercent(NewPercent);

    FNumberFormattingOptions Options = FNumberFormattingOptions().SetMaximumFractionalDigits(0);

    ValueText->SetText(
        FText::Format(
            FTextFormat::FromString(TEXT("{0}/{1}")),
            FText::AsNumber(NewValue, &Options),
            FText::AsNumber(NewMaxValue, &Options)));
}

void UValueGauge::SetAndBoundToGameplayAttribute(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, const FGameplayAttribute& MaxAttribute)
{
    if (ASC)
    {
        bool bFound;
        float Value    = ASC->GetGameplayAttributeValue(Attribute, bFound);
        float MaxValue = ASC->GetGameplayAttributeValue(MaxAttribute, bFound);
        if (bFound)
        {
            SetValue(Value, MaxValue);
        }

        ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::ValueChanged);
        ASC->GetGameplayAttributeValueChangeDelegate(MaxAttribute).AddUObject(this, &ThisClass::MaxValueChanged);
    }
}

void UValueGauge::ValueChanged(const FOnAttributeChangeData& ChangedData)
{
    SetValue(ChangedData.NewValue, CachedMaxValue);
}

void UValueGauge::MaxValueChanged(const FOnAttributeChangeData& ChangedData)
{
    SetValue(CachedValue, ChangedData.NewValue);
}
