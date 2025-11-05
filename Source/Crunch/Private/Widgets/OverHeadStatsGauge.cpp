// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/OverHeadStatsGauge.h"

#include "AbilitySystemComponent.h"

#include "Widgets/ValueGauge.h"
#include "GAS/CAttributeSet.h"

void UOverHeadStatsGauge::ConfigureWithASC(UAbilitySystemComponent* ASC)
{
    if (ASC)
    {
        HealthBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
        ManaBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("OverHeadStatsGauge: %s, ASC is null"), *GetName());
    }
}
