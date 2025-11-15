// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/GameplayWidget.h"
#include "Widgets/ValueGauge.h"
#include "Widgets/AbilityListView.h"


#include "GAS/CAttributeSet.h"
#include "GAS/CAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UGameplayWidget::NativeConstruct()
{
    Super::NativePreConstruct();

    OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());

    if (auto ASC = OwnerASC.Get())
    {
        HealthBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetHealthAttribute(), UCAttributeSet::GetMaxHealthAttribute());
        ManaBar->SetAndBoundToGameplayAttribute(ASC, UCAttributeSet::GetManaAttribute(), UCAttributeSet::GetMaxManaAttribute());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("GameplayWidget: %s, ASC is null : %s"), *GetName(), *GetOwningPlayer()->GetName());
    }
}

void UGameplayWidget::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
    AbilityListView->ConfigureAbilities(Abilities);
}
