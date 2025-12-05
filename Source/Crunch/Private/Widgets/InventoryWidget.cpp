// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/InventoryWidget.h"

#include "Components/WrapBox.h"
#include "Components/WrapBoxSlot.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

#include "GameFramework/PlayerController.h"
#include "Inventory/InventoryItem.h"
#include "Inventory/InventoryComponent.h"
#include "Widgets/InventoryItemWidget.h"
#include "Widgets/InventoryContextMenu.h"

void UInventoryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (!ItemWidgetClass) UE_LOG(LogTemp, Warning, TEXT("%s : ItemWidgetClass is nullptr"), *GetName());

    if (APawn* OwnerPawn = GetOwningPlayerPawn())
    {
        InventoryComponent = OwnerPawn->GetComponentByClass<UInventoryComponent>();
        if (InventoryComponent)
        {
            InventoryComponent->OnItemAdded.AddUObject(this, &ThisClass::ItemAdded);
            InventoryComponent->OnItemRemoved.AddUObject(this, &ThisClass::ItemRemoved);
            InventoryComponent->OnItemStackCountChanged.AddUObject(this, &ThisClass::ItemStackCountChanged);
            InventoryComponent->OnItemAbilityCommitted.AddUObject(this, &ThisClass::ItemAbilityCommitted);




            int Capacity = InventoryComponent->GetCapacity();

            ItemList->ClearChildren();
            ItemWidgets.Empty();

            for (int i = 0; i < Capacity; i++)
            {
                UInventoryItemWidget* NewEmptyWidget = CreateWidget<UInventoryItemWidget>(GetOwningPlayer(), ItemWidgetClass);
                if (NewEmptyWidget)
                {
                    NewEmptyWidget->SetSlotNumber(i);
                    UWrapBoxSlot* NewItemSlot = ItemList->AddChildToWrapBox(NewEmptyWidget);
                    if (NewItemSlot)
                    {
                        NewItemSlot->SetPadding(FMargin(2.f));
                    }

                    ItemWidgets.Add(NewEmptyWidget);

                    NewEmptyWidget->OnInventoryItemDropped.AddUObject(this, &ThisClass::HandleItemDragDrop);

                    NewEmptyWidget->OnLeftButtonClicked.AddUObject(InventoryComponent, &UInventoryComponent::TryActivateItem);
                    NewEmptyWidget->OnRightButtonClicked.AddUObject(this, &ThisClass::ToggleContextMenu);

                    // NewEmptyWidget->OnLeftButtonClicked.AddUObject(this, &ThisClass::LeftButtonClicked);
                }
            }

            SpawnContextMenu();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%s : Owner[%s] InventoryComponent not found"), *GetName(), *OwnerPawn->GetName());
        }
    }
}

void UInventoryWidget::ItemAdded(const UInventoryItem* InventoryItem)
{
    if (!InventoryItem) return;

    if (UInventoryItemWidget* NextAvailableSlot = GetNextAvailableSlot())
    {
        NextAvailableSlot->UpdateInventoryItem(InventoryItem);

        PopulatedItemEntryWidgets.Add(InventoryItem->GetHandle(), NextAvailableSlot);
        if (InventoryComponent)
        {
            InventoryComponent->ItemSlotChanged(InventoryItem->GetHandle(), NextAvailableSlot->GetSlotNumber());
        }
    }
}

UInventoryItemWidget* UInventoryWidget::GetNextAvailableSlot() const
{
    for (UInventoryItemWidget* Widget : ItemWidgets)
    {
        if (Widget->IsEmpty())
        {
            return Widget;
        }
    }

    return nullptr;
}

void UInventoryWidget::ItemStackCountChanged(const FInventoryItemHandle& Handle, int NewCount)
{
    if (UInventoryItemWidget** FoundWidget = PopulatedItemEntryWidgets.Find(Handle))
    {
        (*FoundWidget)->UpdateStackCount();
    }
}

void UInventoryWidget::HandleItemDragDrop(UInventoryItemWidget* DestinationWidget, UInventoryItemWidget* SourceWidget)
{
    if (!DestinationWidget || !SourceWidget) return;

    const UInventoryItem* SourceItem      = SourceWidget->GetInventoryItem();
    const UInventoryItem* DestinationItem = DestinationWidget->GetInventoryItem(); // If the slot is empty, it is nullptr.

    DestinationWidget->UpdateInventoryItem(SourceItem);
    SourceWidget->UpdateInventoryItem(DestinationItem);

    // UE_LOG(LogTemp, Warning, TEXT("HandleItemDragDrop : Src[%s] Dest[%s]"), *GetNameSafe(SourceItem), *GetNameSafe(DestinationItem));

    PopulatedItemEntryWidgets[DestinationWidget->GetItemHandle()] = DestinationWidget;

    if (InventoryComponent)
    {
        InventoryComponent->ItemSlotChanged(DestinationWidget->GetItemHandle(), DestinationWidget->GetSlotNumber());
    }

    if (!SourceWidget->IsEmpty())
    {
        PopulatedItemEntryWidgets[SourceWidget->GetItemHandle()] = SourceWidget;
        if (InventoryComponent)
        {
            InventoryComponent->ItemSlotChanged(SourceWidget->GetItemHandle(), SourceWidget->GetSlotNumber());
        }
    }
}

void UInventoryWidget::ItemRemoved(const FInventoryItemHandle& Handle)
{
    UInventoryItemWidget** FoundWidget = PopulatedItemEntryWidgets.Find(Handle);
    if (FoundWidget && *FoundWidget)
    {
        (*FoundWidget)->EmptySlot();
        PopulatedItemEntryWidgets.Remove(Handle);
    }
}

void UInventoryWidget::SpawnContextMenu()
{
    if (!ContextMenuWidgetClass) return;

    ContextMenuWidget = CreateWidget<UInventoryContextMenu>(this, ContextMenuWidgetClass);
    if (ContextMenuWidget)
    {
        ContextMenuWidget->GetSellButtonClickedEvent().AddDynamic(this, &ThisClass::SellFocusedItem);
        ContextMenuWidget->GetUseButtonClickedEvent().AddDynamic(this, &ThisClass::UseFocusedItem);
        ContextMenuWidget->AddToViewport(1);
        SetContextMenuVisible(false);
    }
}

void UInventoryWidget::SellFocusedItem()
{
    if(InventoryComponent)
    {
        InventoryComponent->SellItem(CurrentFocusedItemHandle);
    }
    SetContextMenuVisible(false);
}

void UInventoryWidget::UseFocusedItem()
{
    if (InventoryComponent)
    {
        InventoryComponent->TryActivateItem(CurrentFocusedItemHandle);
    }
    SetContextMenuVisible(false);
}

void UInventoryWidget::SetContextMenuVisible(bool bVisible)
{
    if (ContextMenuWidget)
    {
        ContextMenuWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
    }
}

void UInventoryWidget::ToggleContextMenu(const FInventoryItemHandle& ItemHandle)
{
    if (!ContextMenuWidget) return;

    if (CurrentFocusedItemHandle == ItemHandle)
    {
        ClearContextMenu();
        return;
    }

    CurrentFocusedItemHandle             = ItemHandle;
    UInventoryItemWidget** ItemWidgetPtr = PopulatedItemEntryWidgets.Find(ItemHandle);
    if (ItemWidgetPtr)
    {
        UInventoryItemWidget* ItemWidget = *ItemWidgetPtr;
        if (ItemWidget)
        {
            SetContextMenuVisible(true);
            FVector2D ItemAbsPos = ItemWidget->GetCachedGeometry().GetAbsolutePositionAtCoordinates(FVector2D{1.f, 0.5f});

            FVector2D ItemWidgetPixelPos, ItemWidgetViewportPos;
            USlateBlueprintLibrary::AbsoluteToViewport(this, ItemAbsPos, ItemWidgetPixelPos, ItemWidgetViewportPos);

            APlayerController* OwningPlayerController = GetOwningPlayer();
            if (OwningPlayerController)
            {
                int ViewportSizeX, ViewportSizeY;
                OwningPlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
                float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

                int Overshoot = ItemWidgetPixelPos.Y + ContextMenuWidget->GetDesiredSize().Y * Scale - ViewportSizeY;
                if (Overshoot > 0)
                {
                    ItemWidgetPixelPos.Y -= Overshoot;
                }
            }

            ContextMenuWidget->SetPositionInViewport(ItemWidgetPixelPos);
        }
    }
}

void UInventoryWidget::ClearContextMenu()
{
    if (ContextMenuWidget)
    {
        ContextMenuWidget->SetVisibility(ESlateVisibility::Hidden);
        CurrentFocusedItemHandle = FInventoryItemHandle::InvalidHandle;
    }
}

void UInventoryWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent)
{
    Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);
    if (!ContextMenuWidget) return;

    if (!NewWidgetPath.ContainsWidget(ContextMenuWidget->GetCachedWidget().Get()))
    {
        ClearContextMenu();
    }
}

void UInventoryWidget::ItemAbilityCommitted(const FInventoryItemHandle& Handle, float CooldownDuration, float TimeRemaining)
{
    UInventoryItemWidget** FoundWidget = PopulatedItemEntryWidgets.Find(Handle);
    if (FoundWidget && *FoundWidget)
    {
        (*FoundWidget)->StartCooldown(CooldownDuration, TimeRemaining);
    }
}
