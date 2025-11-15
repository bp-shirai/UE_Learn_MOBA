// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/CGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"

class UValueGauge;
class UAbilitySystemComponent;
class UGameplayAbility;
class UAbilityListView;


/**
 *
 */
UCLASS(Abstract)
class CRUNCH_API UGameplayWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeConstruct() override;
    void ConfigureAbilities(const TMap<ECAbilityInputID, TSubclassOf<UGameplayAbility>>& Abilities);

private:
    UPROPERTY(meta = (BindWidget))
    UValueGauge* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UValueGauge* ManaBar;

    UPROPERTY(meta = (BindWidget))
	UAbilityListView* AbilityListView;

    UPROPERTY(Transient)
    TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;
};
