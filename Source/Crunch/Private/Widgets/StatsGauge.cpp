// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/StatsGauge.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UStatsGauge::NativePreConstruct()
{
    Super::NativePreConstruct();

    if (Icon && IconTexture)
    {
        Icon->SetBrushFromTexture(IconTexture);
    }
}

void UStatsGauge::NativeConstruct()
{
    Super::NativeConstruct();

    NumberFormatOptions.MaximumFractionalDigits = 0;

    APawn* OwnerPlayer = GetOwningPlayerPawn();

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPlayer);
    if (ASC)
    {
        bool bFound          = false;
        float AttributeValue = ASC->GetGameplayAttributeValue(Attribute, bFound);
        SetValue(AttributeValue);

        ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ThisClass::AttributeChanged);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UStatsGauge::NativeConstruct: Unable to find AbilitySystemComponent from OwnerPlayer"));
    }
}

void UStatsGauge::SetValue(float NewValue)
{
    AttributeText->SetText(FText::AsNumber(NewValue, &NumberFormatOptions));
}

void UStatsGauge::AttributeChanged(const FOnAttributeChangeData& Data)
{
    SetValue(Data.NewValue);
}
