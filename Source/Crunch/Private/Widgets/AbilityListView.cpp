// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/AbilityListView.h"

#include "Widgets/AbilityGauge.h"

#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities)
{
    OnEntryWidgetGenerated().Clear();
    OnEntryWidgetGenerated().AddUObject(this, &ThisClass::AbilityGaugeGenerated);

    ClearListItems();

    for (const auto& [InputID, AbilityClass] : Abilities)
    {
        AddItem(AbilityClass.GetDefaultObject()); // ListViewItem = UGameplayAbility CDO
    }
}

const FAbilityWidgetData* UAbilityListView::FindWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
    if (!AbilityDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("AbilityListView: %s, AbilityDataTable is nullptr"), *GetName());
        return nullptr;
    }

    for (const auto& [AbilityName, AbilityRow] : AbilityDataTable->GetRowMap())
    {
        const FAbilityWidgetData* AbilityData = AbilityDataTable->FindRow<FAbilityWidgetData>(AbilityName, "");
        if (AbilityData && AbilityData->AbilityClass == AbilityClass)
        {
            return AbilityData;
        }
    }

    return nullptr;
}

void UAbilityListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
    UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget);
    if (AbilityGauge)
    {
        AbilityGauge->ConfigureWithWidgetData(FindWidgetDataForAbility(AbilityGauge->GetListItem<UGameplayAbility>()->GetClass()));
    }
}
