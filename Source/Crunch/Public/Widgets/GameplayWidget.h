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
class UStatsGauge;
class UShopWidget;
class UWidgetAnimation;
class UInventoryWidget;
class USkeletalMeshRenderActorWidget;

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

    void ToggleShop();

private:
    UPROPERTY(meta = (BindWidget))
    UValueGauge* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UValueGauge* ManaBar;

    UPROPERTY(meta = (BindWidget))
    UAbilityListView* AbilityListView;

    UPROPERTY(meta = (BindWidget))
    UStatsGauge* Attack;

    UPROPERTY(meta = (BindWidget))
    UStatsGauge* Armor;

    UPROPERTY(meta = (BindWidget))
    UStatsGauge* MoveSpeed;

    UPROPERTY(meta = (BindWidget))
    UStatsGauge* Intelligence;

    UPROPERTY(meta = (BindWidget))
    UStatsGauge* Strength;

    UPROPERTY(meta = (BindWidget))
    UShopWidget* ShopWidget;

    UPROPERTY(meta = (BindWidget))
    UInventoryWidget* InventoryWidget;

    UPROPERTY(meta = (BindWidget))
    USkeletalMeshRenderActorWidget* HeadShotWidget;


    UPROPERTY(Transient, meta = (BindWidgetAnim))
    UWidgetAnimation* ShopPopup_Anim;

    void PlayShopPopupAnim(bool bPlayForward);
    void SetOwningPawnInputEnabled(bool bPawnInputEnabled);
    void SetShowMouseCursor(bool bShowCursor);
    void SetFocusToGameAndUI();
    void SetFocusToGameOnly();

    UPROPERTY(Transient)
    TWeakObjectPtr<UAbilitySystemComponent> OwnerASC;
};
