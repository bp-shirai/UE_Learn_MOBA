// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ValueGauge.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Internationalization/Text.h"

void UValueGauge::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (ProgressBar)
    {
        ProgressBar->SetFillColorAndOpacity(BarColor);
    }
}

void UValueGauge::SetValue(float NewValue, float NewMaxValue)
{
    if (NewMaxValue == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Value Gauge: %s, NewMaxValue can't be 0"), *GetName());
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
