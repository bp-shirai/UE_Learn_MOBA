// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryItemWidget.h"

#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"

#include "Inventory/InventoryItem.h"
#include "Inventory/PA_ShopItem.h"
#include "Widgets/ItemToolTip.h"

void UInventoryItemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
    return !(InventoryItem && InventoryItem->IsValid() && InventoryItem->GetStackCount() > 0);
}

void UInventoryItemWidget::EmptySlot()
{
    InventoryItem = nullptr;

    SetIcon(EmptyTexture);
    // SetToolTip(nullptr);
    SetToolTipWidget(nullptr);

    StackCount->SetVisibility(ESlateVisibility::Hidden);
    CooldownCount->SetVisibility(ESlateVisibility::Hidden);
    CooldownDuration->SetVisibility(ESlateVisibility::Hidden);
    ManaCost->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::UpdateInventoryItem(const UInventoryItem* Item)
{
    InventoryItem = Item;
    //    if (!InventoryItem || !InventoryItem->IsValid() || InventoryItem->GetStackCount() <= 0)
    if (IsEmpty())
    {
        EmptySlot();
        return;
    }

    const UPA_ShopItem* ShopItem = InventoryItem->GetShopItem();

    SetIcon(ShopItem->GetIcon());
    UItemToolTip* ToolTip = SetToolTipWidget(ShopItem);
    if (ToolTip)
    {
        ToolTip->SetPrice(ShopItem->GetSellPrice());
    }

    if (ShopItem->IsStackable())
    {
        StackCount->SetVisibility(ESlateVisibility::Visible);
        UpdateStackCount();
    }
    else
    {
        StackCount->SetVisibility(ESlateVisibility::Hidden);
    }
}

void UInventoryItemWidget::UpdateStackCount()
{
    if (InventoryItem)
    {
        StackCount->SetText(FText::AsNumber(InventoryItem->GetStackCount()));
    }
}
