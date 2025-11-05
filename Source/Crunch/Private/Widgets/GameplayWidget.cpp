// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/GameplayWidget.h"
#include "Widgets/ValueGauge.h"
#include "GAS/CAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UGameplayWidget::NativeConstruct()
{
    Super::NativePreConstruct();

    UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
    if (ASC)
    {
        OwnerASC = ASC;
        HealthBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
        ManaBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameplayWidget: %s, ASC is null : %s"), *GetName(), *GetOwningPlayer()->GetName());
    }
}
