// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryItemWidget.h"

#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"

#include "Inventory/InventoryItem.h"
#include "Inventory/PA_ShopItem.h"
#include "Widgets/ItemToolTip.h"
#include "Widgets/InventoryItemDragDropOp.h"

void UInventoryItemWidget::NativeConstruct()
{
    Super::NativeConstruct();

    EmptySlot();
}

bool UInventoryItemWidget::IsEmpty() const
{
    return !InventoryItem || !(InventoryItem->IsValid());
}

void UInventoryItemWidget::EmptySlot()
{
    InventoryItem = nullptr;

    SetIcon(EmptyTexture);
    SetToolTip(nullptr);

    StackCount->SetVisibility(ESlateVisibility::Hidden);
    CooldownCount->SetVisibility(ESlateVisibility::Hidden);
    CooldownDuration->SetVisibility(ESlateVisibility::Hidden);
    ManaCost->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryItemWidget::UpdateInventoryItem(const UInventoryItem* Item)
{
    InventoryItem = Item;
    if (!InventoryItem || !InventoryItem->IsValid() || InventoryItem->GetStackCount() <= 0)
    // if (IsEmpty())
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

UTexture2D* UInventoryItemWidget::GetIconTexture() const
{
    if (!IsEmpty())
    {
        return InventoryItem->GetShopItem()->GetIcon();
    }

    return nullptr;
}

void UInventoryItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
    Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

    if (!IsEmpty() && DragDropClass)
    {
        UInventoryItemDragDropOp* DragDropOp = NewObject<UInventoryItemDragDropOp>(this, DragDropClass);
        if (DragDropOp)
        {
            DragDropOp->SetDraggedItem(this);
            OutOperation = DragDropOp;
        }
    }
}

bool UInventoryItemWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
    if (UInventoryItemWidget* OtherWidget = Cast<UInventoryItemWidget>(InOperation->Payload))
    {
        if (OtherWidget && !OtherWidget->IsEmpty())
        {
            OnInventoryItemDropped.Broadcast(this, OtherWidget);
            return true;
        }
    }

    return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

FInventoryItemHandle UInventoryItemWidget::GetItemHandle() const
{
    if (!IsEmpty())
    {
        return InventoryItem->GetHandle();
    }

    return FInventoryItemHandle::InvalidHandle;
}

void UInventoryItemWidget::RightButtonClicked()
{
    if (!IsEmpty())
    {
        OnRightButtonClicked.Broadcast(GetItemHandle());
    }
}

void UInventoryItemWidget::LeftButtonClicked()
{
    if (!IsEmpty())
    {
        OnLeftButtonClicked.Broadcast(GetItemHandle());
    }
}
