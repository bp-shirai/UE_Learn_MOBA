// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryItemWidget.h"

#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

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
    ClearCooldown();
    UnBindCanCastAbilityDelegate();

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
    UnBindCanCastAbilityDelegate();

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

    ClearCooldown();

    if (InventoryItem->IsGrantingAnyAbility())
    {
        UpdateCanCastDisplay(InventoryItem->CanCastAbility());
        float AbilityCooldownRemaining = InventoryItem->GetAbilityCooldownTimeRemaining();
        float AbilityCooldownDuration  = InventoryItem->GetAbilityCooldownDuration();
        if (AbilityCooldownRemaining > 0.f)
        {
            StartCooldown(AbilityCooldownDuration, AbilityCooldownRemaining);
        }

        float AbilityCost = InventoryItem->GetAbilityManaCost();
        ManaCost->SetVisibility(AbilityCost == 0.f ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
        ManaCost->SetText(FText::AsNumber(AbilityCost));
        CooldownDuration->SetVisibility(AbilityCooldownDuration == 0.f ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
        CooldownDuration->SetText(FText::AsNumber(AbilityCooldownDuration));

        BindCanCastAbilityDelegate();
    }
    else
    {
        UpdateCanCastDisplay(true);
        ManaCost->SetVisibility(ESlateVisibility::Hidden);
        CooldownDuration->SetVisibility(ESlateVisibility::Hidden);
        CooldownCount->SetVisibility(ESlateVisibility::Hidden);
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

#pragma region---------------- GAS ------------------------------------------------------

void UInventoryItemWidget::StartCooldown(float InCooldownDuration, float InTimeRemaining)
{
    CooldownTimeRemaining = InTimeRemaining;
    CooldownTimeDuration  = InCooldownDuration;

    GetWorld()->GetTimerManager().SetTimer(Handle_CooldownDuration, this, &ThisClass::CooldownFinished, CooldownTimeRemaining);
    GetWorld()->GetTimerManager().SetTimer(Handle_CooldownUpdate, this, &ThisClass::UpdateCooldown, CooldownUpdateInterval, true);

    CooldownCount->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryItemWidget::CooldownFinished()
{
    GetWorld()->GetTimerManager().ClearTimer(Handle_CooldownUpdate);
    CooldownCount->SetVisibility(ESlateVisibility::Hidden);
    if (GetItemIcon())
    {
        GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(MaterialParam_CooldownAmt, 1.f);
    }
}

void UInventoryItemWidget::UpdateCooldown()
{
    CooldownTimeRemaining -= CooldownUpdateInterval;
    const float CooldownAmt                   = 1.f - CooldownTimeRemaining / CooldownTimeDuration;
    CooldownFormatOpt.MaximumFractionalDigits = CooldownTimeRemaining > 1.f ? 0 : 1;
    CooldownCount->SetText(FText::AsNumber(CooldownTimeRemaining, &CooldownFormatOpt));
    
    if (GetItemIcon())
	{
		GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(MaterialParam_CooldownAmt, CooldownAmt);
	}
}

void UInventoryItemWidget::ClearCooldown()
{
    CooldownFinished();
}

void UInventoryItemWidget::SetIcon(UTexture2D* IconTexture)
{
    if (GetItemIcon())
    {
        GetItemIcon()->GetDynamicMaterial()->SetTextureParameterValue(MaterialParam_IconTexture, IconTexture);
        return;
    }

    Super::SetIcon(IconTexture);
}

#pragma endregion

void UInventoryItemWidget::UpdateCanCastDisplay(bool bCanCast)
{
    GetItemIcon()->GetDynamicMaterial()->SetScalarParameterValue(MaterialParam_CanCast, bCanCast ? 1.f : 0.f);
}

void UInventoryItemWidget::BindCanCastAbilityDelegate()
{
    if (InventoryItem)
    {
        const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.AddUObject(this, &ThisClass::UpdateCanCastDisplay);
    }
}

void UInventoryItemWidget::UnBindCanCastAbilityDelegate()
{
    if (InventoryItem)
    {
        const_cast<UInventoryItem*>(InventoryItem)->OnAbilityCanCastUpdated.RemoveAll(this);
    }
}
