// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ListView.h"

#include "GAS/CGameplayAbilityTypes.h"
#include "AbilityListView.generated.h"

class UGameplayAbility;
struct FAbilityWidgetData;

/**
 *
 */
UCLASS()
class CRUNCH_API UAbilityListView : public UListView
{
    GENERATED_BODY()

public:
    void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);

private:
    UPROPERTY(EditAnywhere, Category = "Data")
    UDataTable* AbilityDataTable;

    void AbilityGaugeGenerated(UUserWidget& Widget);

    const FAbilityWidgetData* FindWidgetDataForAbility(const TSubclassOf<UGameplayAbility>& AbilityClass) const;
};
