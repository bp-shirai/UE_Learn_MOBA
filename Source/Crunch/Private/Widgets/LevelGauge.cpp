// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/LevelGauge.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "GAS/CHeroAttributeSet.h"

void ULevelGauge::NativeConstruct()
{
    Super::NativeConstruct();

    NumberFormattingOptions.MaximumFractionalDigits = 0;

    OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    if (OwnerASC)
    {
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
        OwnerASC->GetGameplayAttributeValueChangeDelegate(UCHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ThisClass::UpdateGauge);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("LevelGauge: %s, ASC is null : %s"), *GetName(), *GetOwningPlayer()->GetName());
    }

    UpdateGauge(FOnAttributeChangeData());
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& ChangedData)
{
    if (!OwnerASC) return;

    float CurrentExp   = OwnerASC->GetNumericAttribute(UCHeroAttributeSet::GetExperienceAttribute());
    float NextLevelExp = OwnerASC->GetNumericAttribute(UCHeroAttributeSet::GetNextLevelExperienceAttribute());
    float PrevLevelExp = OwnerASC->GetNumericAttribute(UCHeroAttributeSet::GetPrevLevelExperienceAttribute());
    float Level        = OwnerASC->GetNumericAttribute(UCHeroAttributeSet::GetLevelAttribute());

    LevelText->SetText(FText::AsNumber(Level, &NumberFormattingOptions));

    float Progress    = CurrentExp - PrevLevelExp;
    float LevelExpAmt = NextLevelExp - PrevLevelExp;
    float Percent     = Progress / LevelExpAmt;

    if (NextLevelExp == 0) // Max Level
    {
        Percent = 1.f;
    }

    if (LevelProgressImage)
    {
        LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(PercentMaterialParamName, Percent);
    }
}
