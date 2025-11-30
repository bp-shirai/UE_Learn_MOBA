// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/GameplayWidget.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/PlayerController.h"
#include "Widgets/ValueGauge.h"
#include "Widgets/AbilityListView.h"
#include "Widgets/CShopWidget.h"

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

void UGameplayWidget::ToggleShop()
{
    if (ShopWidget->GetVisibility() == ESlateVisibility::HitTestInvisible)
    {
        ShopWidget->SetVisibility(ESlateVisibility::Visible);
        PlayShopPopupAnim(true);
        SetOwningPawnInputEnabled(false);
        SetShowMouseCursor(true);
        SetFocusToGameAndUI();
        ShopWidget->SetFocus();
    }
    else
    {
        ShopWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
        PlayShopPopupAnim(false);
        SetOwningPawnInputEnabled(true);
        SetShowMouseCursor(false);
        SetFocusToGameOnly();
    }
}

void UGameplayWidget::PlayShopPopupAnim(bool bPlayForward)
{
    if (bPlayForward)
    {
        PlayAnimationForward(ShopPopup_Anim);
    }
    else
    {
        PlayAnimationReverse(ShopPopup_Anim);
    }
}

void UGameplayWidget::SetOwningPawnInputEnabled(bool bPawnInputEnabled)
{
    if (bPawnInputEnabled)
    {
        GetOwningPlayerPawn()->EnableInput(GetOwningPlayer());
    }
    else
    {
        GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
    }
}

void UGameplayWidget::SetShowMouseCursor(bool bShowCursor)
{
    GetOwningPlayer()->SetShowMouseCursor(bShowCursor);
}

void UGameplayWidget::SetFocusToGameAndUI()
{
    FInputModeGameAndUI InputMode;
    InputMode.SetHideCursorDuringCapture(false);
    GetOwningPlayer()->SetInputMode(InputMode);
}

void UGameplayWidget::SetFocusToGameOnly()
{
    FInputModeGameOnly InputMode;
    GetOwningPlayer()->SetInputMode(InputMode);
}
